/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <set>
#include <et/core/singleton.h>
#include <et/app/events.h>

namespace et
{
    
	class MailComposerPrivate;
    class MailComposer : public Singleton<MailComposer>
    {
    public:
		bool canSendEmail() const;

        void composeEmail(const std::string& recepient,
			const std::string& title, const std::string& text);

		void attachFile(const std::string& attachmentFileName);

		void present();

	private:
		MailComposer();
        ~MailComposer();
		ET_SINGLETON_COPY_DENY(MailComposer);
		
	private:
		friend class MailComposerPrivate;
		
	private:
		MailComposerPrivate* _private;
	};
	
	inline MailComposer& mailComposer()
		{ return MailComposer::instance(); }
}
