/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/app/events.h>

namespace et
{
 class TextureObject;
 class ProgramObject;

 class ResourceStorage : public EventReceiver
 {
  public:
   ResourceStorage();
   virtual ~ResourceStorage();

   void manageTextureObject(const TextureObject& resource);
   void discardTextureObject(const TextureObject& resource);

   void manageProgramObject(const ProgramObject& resource);
   void discardProgramObject(const ProgramObject& resource);
   
  private:
   class ResourceStoragePrivate;
   friend class ResourceStoragePrivate;
   ResourceStoragePrivate* _private;
 };
}