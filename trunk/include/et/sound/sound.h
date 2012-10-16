/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <string>
#include <et/core/singleton.h>
#include <et/core/intrusiveptr.h>

namespace et
{
    namespace audio
    {
        class TrackPrivate;
        class Track : public Shared
        {
        public:
            typedef IntrusivePtr<Track> Pointer;
            
            Track(const std::string& fileName);
            
        public:
            friend class TrackPivate;
            TrackPrivate* _private;
        };
        
        /*
         * Player
         */
        
        class PlayerPrivate;
        class Player : public Shared
        {
        public:
            typedef IntrusivePtr<Player> Pointer;
            
        public:
            Player();
            Player(Track::Pointer track);
            
            void play(Track::Pointer);
            
        private:
            friend class PlayerPrivate;
            PlayerPrivate* _private;
        };
        
        /*
         * Manager
         */
        
        class ManagerPrivate;
        class Manager : public Singleton<Manager>
        {
        public:
            
        private:
            Manager();
            
        private:
            ET_SINGLETON_COPY_DENY(Manager);
            
        private:
            friend class ManagerPrivate;
            ManagerPrivate* _private;
        };
    }
}