#pragma once
#ifndef OPENGM_DATASET_HXX
#define OPENGM_DATASET_HXX

#include <vector>
#include <cstdlib>

#include "../../graphicalmodel/weights.hxx"
#include "../loss/noloss.hxx"

namespace opengm {
   namespace datasets{

     template<class GM, class LOSS=opengm::learning::NoLoss>
      class Dataset{
      public:
         typedef GM                       GMType;
         typedef GM                       GMWITHLOSS;
         typedef LOSS                     LossType;
         typedef typename LOSS::Parameter LossParameterType;
         typedef typename GM::ValueType   ValueType;
         typedef typename GM::IndexType   IndexType;
         typedef typename GM::LabelType   LabelType;
         typedef opengm::learning::Weights<ValueType> Weights;

         bool                          lockModel(const size_t i)               { ++count_[i]; }
         bool                          unlockModel(const size_t i)             { OPENGM_ASSERT(count_[i]>0); --count_[i]; }
         const GM&                     getModel(const size_t i) const          { return gms_[i]; } 
         const GMWITHLOSS&             getModelWithLoss(const size_t i)const   { return gmsWithLoss_[i]; }
         const LossParameterType&      getLossParameters(const size_t i)const  { return lossParams_[i]; }
         const std::vector<LabelType>& getGT(const size_t i) const             { return gts_[i]; }
         Weights&                      getWeights()                            { return weights_; } 
         size_t                        getNumberOfWeights() const              { return weights_.numberOfWeights(); }
         size_t                        getNumberOfModels() const               { return gms_.size(); } 
         
         Dataset(size_t numInstances=0);
        //void loadAll(std::string path,std::string prefix); 
 
        friend class DatasetSerialization;
        // friend void loadAll<Dataset<GM,LOSS> > (const std::string datasetpath, const std::string prefix, Dataset<GM,LOSS>& ds);

     protected:	
         std::vector<size_t> count_;
         std::vector<bool> isCached_;
         std::vector<GM> gms_; 
         std::vector<GMWITHLOSS> gmsWithLoss_; 
         std::vector<LossParameterType> lossParams_;
         std::vector<std::vector<LabelType> > gts_;
         Weights weights_;

         void buildModelWithLoss(size_t i);
      };
      


      template<class GM, class LOSS>
      Dataset<GM, LOSS>::Dataset(size_t numInstances)
          : count_(std::vector<size_t>(numInstances)),
            isCached_(std::vector<bool>(numInstances)),
            gms_(std::vector<GM>(numInstances)),
            gmsWithLoss_(std::vector<GMWITHLOSS>(numInstances)),
            gts_(std::vector<std::vector<LabelType> >(numInstances)),
            weights_(Weights(0)),
            lossParams_(std::vector<LossParameterType>(numInstances))
      {
      }; 


     template<class GM, class LOSS>
     void Dataset<GM, LOSS>::buildModelWithLoss(size_t i){
         gmsWithLoss_[i] = gms_[i];
         LOSS loss(lossParams_[i]);
         loss.addLoss(gmsWithLoss_[i], gts_[i].begin());
      }

/*
     template<class GM, class LOSS>
     void Dataset<GM, LOSS>::loadAll(std::string datasetpath,std::string prefix){

         //Load Header 
         std::stringstream hss;
         hss << datasetpath << "/"<<prefix<<"info.h5";
         hid_t file =  marray::hdf5::openFile(hss.str());
         std::vector<size_t> temp(1);
         marray::hdf5::loadVec(file, "numberOfWeights", temp);
         size_t numWeights = temp[0];
         marray::hdf5::loadVec(file, "numberOfModels", temp);
         size_t numModel = temp[0];
         marray::hdf5::closeFile(file);
         
         gms_.resize(numModel); 
	 gmsWithLoss_.resize(numModel);
         gt_.resize(numModel);
         weights_ = Weights(numWeights);
         //Load Models and ground truth
         for(size_t m=0; m<numModel; ++m){
            std::stringstream ss;
            ss  << datasetpath <<"/"<<prefix<<"gm_" << m <<".h5"; 
            hid_t file =  marray::hdf5::openFile(ss.str()); 
            marray::hdf5::loadVec(file, "gt", gt_[m]);
            marray::hdf5::closeFile(file);
            opengm::hdf5::load(gms_[m],ss.str(),"gm"); 
	    buildModelWithLoss(m);
         }

      };
*/
   }
} // namespace opengm

#endif 
