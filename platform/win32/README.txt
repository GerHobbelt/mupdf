This MSVC project needs the thirdparty sources to be in place.

## Notes

- For OCR to work, set the environment variable `TESSDATA_PREFIX` to point at one of the the tessdata repositories in `./thirdparty/`, e.g.:

  ```
  TESSDATA_PREFIX=$(ProjectDir)..\..\thirdparty\tessdata_best
  ```
  