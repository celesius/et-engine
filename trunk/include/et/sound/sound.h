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
#include <et/sound/formats.h>

namespace et
{
    namespace audio
    {
		class Player;
		class Manager;

        class TrackPrivate;
        class Track : public Shared
        {
        public:
            typedef IntrusivePtr<Track> Pointer;
            
		public:
			~Track();

		private:
            Track(const std::string& fileName);
			Track(Description::Pointer data);
			ET_SINGLETON_COPY_DENY(Track);

		private:
			void init(Description::Pointer data);
            
		private:
			friend class Player;
			friend class Manager;
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
			~Player();

			void play(bool looped = false);
            void play(Track::Pointer, bool looped = false);
			void pause();
			void stop();

			void setVolume(float);

        private:
			Player();
            Player(Track::Pointer track);
			ET_SINGLETON_COPY_DENY(Player);

			void init();
			void linkTrack(Track::Pointer);
            
        private:
			friend class Manager;
            friend class PlayerPrivate;

		private:
            PlayerPrivate* _private;
			Track::Pointer _currentTrack;
        };
        
        /*
         * Manager
         */
        
        class ManagerPrivate;
        class Manager : public Singleton<Manager>
        {
        public:
			~Manager();

			Track::Pointer loadTrack(const std::string& fileName);
			Track::Pointer genTrack(Description::Pointer desc);

			Player::Pointer genPlayer(Track::Pointer track);
			Player::Pointer genPlayer();

			bool checkErrors();

        private:
            Manager();
           
        private:
            ET_SINGLETON_COPY_DENY(Manager);
            
        private:
            friend class ManagerPrivate;
            ManagerPrivate* _private;
        };

		inline Manager& manager()
			{ return Manager::instance(); }
    }
}