/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/platform-ios/imagepicker.h>

@interface ImagePickerProxy : NSObject<UIImagePickerControllerDelegate>
{
	UIImagePickerController* _picker;
	et::ImagePickerPrivate* _p;
}

- (id)initWithPrivate:(et::ImagePickerPrivate*)p;

@end

namespace et
{
	class ImagePickerPrivate
	{
	public:
		ImagePickerPrivate();
		~ImagePickerPrivate();
		
		void pick(ImagePickerSource s);
		
	public:
		ImagePicker* owner;
		ImagePickerProxy* proxy;
	};
}

using namespace et;

ImagePicker::ImagePicker() :
	_private(new ImagePickerPrivate)
{
	_private->owner = this;
}

ImagePicker::~ImagePicker()
{
	delete _private;
}

void ImagePicker::selectImage(ImagePickerSource souce)
{
	
}

/*
 * Image Picker Private
 */
ImagePickerPrivate::ImagePickerPrivate()
{
	proxy = [[ImagePickerProxy alloc] initWithPrivate:this];
}

ImagePickerPrivate::~ImagePickerPrivate()
{
	[proxy release];
}

void ImagePickerPrivate::pick(ImagePickerSource s)
{
}

/*
 * Image Picker Proxy
 */

@implementation ImagePickerProxy

- (id)initWithPrivate:(et::ImagePickerPrivate*)p
{
	self = [super init];
	if (self)
	{
		_p = p;
		_picker = [[UIImagePickerController alloc] init];
	}
	return self;
}

- (void)dealloc
{
	[_picker release];
	[super dealloc];
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
	
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
	
}

@end