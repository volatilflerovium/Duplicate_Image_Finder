# Duplicate Image Finder

Duplicate Image Finder is a simple application written in C++ and packed
as [AppImage](https://appimage.org/) that can find duplicated images.

# What is an AppImage?

An AppImage is a downloadable file for Linux that contains an application and everything the application needs to run (e.g., libraries, icons, fonts, translations, etc.) that cannot be reasonably expected to be part of each target system.

# How to run and install Duplicate Image Finder?

Download [Duplicate Image Finder](https://github.com/volatilflerovium/Duplicate_Image_Finder/raw/main/wxDuplicate_Image_Finder-x86_64.AppImage) and make it executable. 
You can do this using the graphical user interface, or the terminal emulator. 
Via a terminal run:
```
chmod ugo x ~/path/to/wxDuplicate_Image_Finder-x86_64.AppImage
```
Using the graphical interface: right-clicking on it, select Properties , 
and then on the Permissions tab , check the box next to Allow executing file as program.

# Supported files
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

# The power of the humble histogram

Some methods to compare two images are:

*	Hashing:
		Two identical files at binary level will produce the same hash sum.

*	MSE:
		Mean Squared Error between the two images is the sum of the squared difference between the two images. The lower the error, the more “similar” the images are.

*	SSIM:
		Structural Similarity Index method is more involved than the MSE method, 
		but the gist is that SSIM attempts to model the perceived change in the 
		structural information of the image, whereas MSE is actually estimating 
		the perceived errors.

*	AI:
		There are deep learning-based image similarity methods that we can utilize, particularly siamese networks. Siamese networks are super powerful models that can be trained with very little data to compute accurate image similarity scores.

It would have been easy to implement any of these methods, after all everybody can
copy-and-paste. Instead, Duplicate Image Finder implement a simple algorithm to find duplicated
images which is invariant to rotations, reflexion and, to some degree, size.

For each image Duplicate Image Finder calculates a set of concentric mask
proportional to the image and use these to get a set of histograms.

We have two basic consequences of this:

* if one image is a rotated or a reflected version of another, both 
will have the same set of histograms.

* each concentric histogram contains information of the images of a particular
region of it. Globally, two images can have the same histogram, however
locally they can have different histograms.

# GUI

The GUI is built using [wxWidgets](https://www.wxwidgets.org).

In the following image we see the components of the GUI:

![Example](http://103.102.44.126/img/dpf0.png)
