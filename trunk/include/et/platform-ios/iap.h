/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <set>

namespace et
{
    class PurchasesManagerDelegate
    {
    public:
        virtual ~PurchasesManagerDelegate() { }
        
        virtual void purchasesManagerDidCheckAvailableProducts(const StringList& products) { }
        virtual void purchasesManagerDidFailToCheckAvailableProducts() { }
        
        virtual void purchasesManagerDidPurchaseProduct(const std::string& product) { }
        virtual void purchasesManagerDidFailToPurchaseProduct(const std::string& product) { }
        
        virtual void purchasesManagerDidRestorePurchasedProduct(const std::string& product) { }
        virtual void purchasesManagerDidFinishRestoringPurchases() { }
        virtual void purchasesManagerDidFailToRestorePurchases() { }
    };
    
    class PurchasesManager
    {
    public:
        typedef std::set<std::string> ProductsSet;
        
    public:
        PurchasesManager();
        ~PurchasesManager();
        
        void checkAvailableProducts(const ProductsSet& products, PurchasesManagerDelegate* delegate);
		void restoreTransactions(PurchasesManagerDelegate* delegate);
		
        bool purchaseProduct(const std::string& product, PurchasesManagerDelegate* delegate);
    };
}
