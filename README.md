# Duplicate Image Finder

Duplicate Image Finder is a simple application written in C++ and packed
as [AppImage](https://appimage.org/) that can find duplicated images.

## What is an AppImage?

An AppImage is a downloadable file for Linux that contains an application and everything the application needs to run (e.g., libraries, icons, fonts, translations, etc.) that cannot be reasonably expected to be part of each target system.

## How to run and install Duplicate Image Finder?

Download [Duplicate Image Finder](https://github.com/volatilflerovium/Duplicate_Image_Finder/raw/main/wxDuplicate_Image_Finder-x86_64.AppImage) and make it executable. 
You can do this using the graphical user interface, or the terminal emulator. 
Via a terminal run:
```
chmod ugo x ~/path/to/DuplicateImageFinder-x86_64.AppImage
```

Using the graphical interface: right-clicking on it, select Properties , 
and then on the Permissions tab , check the box next to Allow executing file as program.

## Supported files
As it uses OpenCV, it supports the following formats:

* Windows bitmaps - *.bmp, *.dib
* JPEG files - *.jpeg, *.jpg, *.jpe
* JPEG 2000 files - *.jp2
* Portable Network Graphics - *.png
* WebP - *.webp
* Portable image format - *.pbm, *.pgm, *.ppm *.pxm, *.pnm
* PFM files - *.pfm
* Sun rasters - *.sr, *.ras
* TIFF files - *.tiff, *.tif
* OpenEXR Image files - *.exr
* Radiance HDR - *.hdr, *.pic
* Raster and Vector geospatial data supported by GDAL


It uses OpenCV and wxWidgets.
