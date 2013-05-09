/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <set>

#include <et/core/hierarchy.h>
#include <et/core/flags.h>
#include <et/core/transformable.h>
#include <et/core/serialization.h>
#include <et/scene3d/material.h>
#include <et/scene3d/serialization.h>

namespace et
{
	namespace s3d
	{
		class Element;
		class Scene3dStorage;

		enum ElementFlags
		{
			ElementFlag_Renderable = 0x0001,
		};

		enum ElementType
		{
			ElementType_Container,
			ElementType_Mesh,
			ElementType_SupportMesh,
			ElementType_Camera,
			ElementType_Light,
			// ... to be addded ...
			ElementType_Storage = 0xfe,
			ElementType_Any = 0xff,
			ElementType_Custom
		};

		class ElementFactory;
		typedef Hierarchy<Element> ElementHierarchy;
		class Element : public ElementHierarchy, public Flags, public ComponentTransformable
		{
		public:
			typedef IntrusivePtr<Element> Pointer;

			size_t tag;
			virtual ElementType type() const = 0;
			virtual Element* duplicate() = 0;

		public:
			Element(const std::string& name, Element* parent);

			void setName(const std::string& name)
				{ _name = name; }

			const std::string& name() const
				{ return _name; }

			bool active() const
				{ return _active; }

			void setActive(bool active);

			bool isKindOf(ElementType t) const;

			virtual mat4 finalTransform();
			void invalidateTransform();

			void setParent(Element* p);

			Pointer childWithName(const std::string& name, ElementType ofType = ElementType_Any,
				bool assertFail = false);
			
			Element::List childrenOfType(ElementType ofType) const;
			Element::List childrenHavingFlag(size_t flag);

			virtual void serialize(std::ostream& stream, SceneVersion version);
			virtual void deserialize(std::istream& stream, ElementFactory* factory, SceneVersion version);
			
			void clear();

			const std::set<std::string>& properties() const
				{ return _properites; }

			std::set<std::string>& properties()
				{ return _properites; }

			void addPropertyString(const std::string& s)
				{ _properites.insert(s); }
			
			bool hasPropertyString(const std::string& s) const;
			
		protected:
			void serializeGeneralParameters(std::ostream& stream, SceneVersion version);
			void serializeChildren(std::ostream& stream, SceneVersion version);
			void deserializeGeneralParameters(std::istream& stream, SceneVersion version);
			void deserializeChildren(std::istream& stream, ElementFactory* factory, SceneVersion version);

			void duplicateChildrenToObject(Element* object);
			void duplicateBasePropertiesToObject(Element* object);

		private:
			Pointer childWithNameCallback(const std::string& name, Pointer root, ElementType ofType);
			void childrenOfTypeCallback(ElementType t, Element::List& list, Pointer root) const;
			void childrenHavingFlagCallback(size_t flag, Element::List& list, Pointer root);

		private:
			std::string _name;
			std::set<std::string> _properites;
			mat4 _cachedFinalTransform;
			bool _active;
		};

		class ElementContainer : public Element
		{
		public:
			typedef IntrusivePtr<ElementContainer> Pointer;

		public:
			ElementContainer(const std::string& name, Element* parent) : Element(name, parent) 
				{ }

			ElementType type() const 
				{ return ElementType_Container; }

			ElementContainer* duplicate()
			{
				ElementContainer* result = new ElementContainer(name(), parent());
				duplicateChildrenToObject(result);
				result->tag = tag;
				return result; 
			}

			void serialize(std::ostream& stream, SceneVersion version)
			{
				serializeGeneralParameters(stream, version);
				serializeChildren(stream, version);
			}

			void deserialize(std::istream& stream, ElementFactory* factory, SceneVersion version)
			{
				deserializeGeneralParameters(stream, version);
				deserializeChildren(stream, factory, version);
			}
		};

		class RenderableElement : public Element
		{
		public:
			typedef IntrusivePtr<RenderableElement> Pointer;

		public:
			RenderableElement(const std::string& name, Element* parent) : Element(name, parent),
				_visible(true) { setFlag(ElementFlag_Renderable); }

			Material& material() 
				{ return _material; }

			const Material& material() const
				{ return _material; }

			void setMaterial(const Material& material)
				{ _material = material; }

			bool visible() const
				{ return _visible; }
			
			void setVisible(bool visible)
				{ _visible = visible; }
			
		private:
			Material _material;
			bool _visible;
		};

		class CustomElementFactory
		{
		public:
			virtual Element::Pointer createElementOfType(size_t type, Element* parent) = 0;
		};

		class ElementFactory : public CustomElementFactory
		{
		public:
			virtual Material materialWithId(int id) = 0;
			virtual VertexArrayObject vaoWithIdentifiers(const std::string& vbid, const std::string& ibid) = 0;
		};

	}
}