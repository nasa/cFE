import os
import shutil

pluginsInfo = os.listdir('./plugins/info')
for file_name in pluginsInfo:
    full_file_name = os.path.join('./plugins/info', file_name)
    if os.path.isfile(full_file_name):
        shutil.copy(full_file_name, "../../../cfs_gw/tools/ctf/plugins/info")
        
pluginsCfs = os.listdir("./plugins/cfs")
for file_name in pluginsCfs:
    full_file_name = os.path.join('./plugins/cfs', file_name)
    if os.path.isfile(full_file_name):
        shutil.copy(full_file_name, "../../../cfs_gw/tools/ctf/plugins/cfs")      
        
plugins = os.listdir("./plugins")
for file in plugins:
   full_file_name = os.path.join('./plugins', file)
   if full_file_name != os.path.join('./plugins',"cfs") and full_file_name != os.path.join('./plugins',"info"):
      newFileName = os.path.join('../../../cfs_gw/tools/ctf/plugins', file)
      if os. path. isdir(newFileName):
          shutil.rmtree(newFileName)
      shutil.copytree(full_file_name, newFileName)       
      print(os.path.join('./plugins', file)) 
