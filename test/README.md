# Test code


## Test

 **1. Load Pretrained Models** (*Search ```load_model()``` function*)
  
  Set 
  
  ```
  folder_prefix = 'testresults'
  model_str = "118_15"
  integral_model_str = "125_4"
  batch_size = 10
  ```
  
  By default, it loads data from ```testset/``` where human images are located. Human figures can be of any size.
  Search *class JointDataset* for details. 
  
 **2. Run*** ```python test.py```
 
 **3. Save results** The results are automatically exported to ```D:/results/ + folder_prefix + '/'```. First, 2d predictions overlaid on test images are saved to this directory. Further, 2d and 3d will be saved to subfolder ```2d/``` and ```3d/``` respectively. Some joints are missing (*invisible*), just like **MPII**, and so the results might be bizarre. But it doesn't matter since they are invisible.