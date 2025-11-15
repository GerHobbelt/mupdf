# How to update the MSVC project settings for all projects at once

```sh
for f in *.vcxproj ; do
  node ./update-vcxproj $f
done
```



