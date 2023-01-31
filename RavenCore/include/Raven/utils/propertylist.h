#ifndef _RAVEN_UTILS_PROPERTYLIST_H_
#define _RAVEN_UTILS_PROPERTYLIST_H_

#include<Raven/core/math.h>
#include<Raven/core/spectrum.h>
#include<Raven/core/transform.h>
#include<Raven/core/base.h>
#include<optional>
#include<queue>

namespace Raven {
	struct ObjectRef {
	public:
		inline Ref<RavenObject> getRef()const {
			return my_ref;
		}

		inline bool matchType(const std::string& type) {
			return type == my_type;
		}
		ObjectRef() :my_type("nullptr"), my_ref(nullptr) {}
		ObjectRef(const std::string& type, const Ref<RavenObject>& ref) :my_type(type), my_ref(ref) {}
	private:
		std::string my_type;
		Ref<RavenObject> my_ref;
	};

	class PropertyList {

		enum class RefType {
			Ref,
			RefById
		};

	public:
		PropertyList() :refCount(0) {}

		void setBoolean(const std::string&, const bool& value);
		void setInteger(const std::string&, const int& value);
		void setFloat(const std::string&, const double& value);
		void setVector2f(const std::string&, const Vector2f& value);
		void setPoint2f(const std::string&, const Point2f& value);
		void setPoint3f(const std::string&, const Point3f& value);
		void setVector3f(const std::string&, const Vector3f& value);
		void setNormal3f(const std::string&, const Normal3f& value);
		void setSpectra(const std::string&, const Spectrum& value);
		void setString(const std::string&, const std::string& value);
		void setObjectRef(const std::string& type, const Ref<RavenObject>& ref);

		static void setObjectRefById(const std::string&refid, const std::string&type, 
			const Ref<RavenObject>&ref, PropertyList&currentPropertyList);

		bool getBoolean(const std::string&, const bool&)const;
		int getInteger(const std::string&, const int&)const;
		double getFloat(const std::string&, const double&)const;
		Point2f getPoint2f(const std::string&, const Point2f&)const;
		Vector2f getVector2f(const std::string&, const Vector2f&)const;
		Point3f getPoint3f(const std::string&, const Point3f&)const;
		Vector3f getVector3f(const std::string&, const Vector3f&)const;
		Normal3f getNormal3f(const std::string&, const Normal3f&)const;
		Spectrum getSpectra(const std::string&, const Spectrum&)const;
		std::string getString(const std::string&, const std::string&)const;

		ObjectRef getObjectRef(int)const;


		static void clearCache() {
			auto& refMap = getRefMap();
			refMap.erase(refMap.begin(), refMap.end());
		}

	private:

		static std::map<std::string, ObjectRef>& getRefMap() {
			static std::map<std::string, ObjectRef> refMap;//用于存放声明在外部的指针
			return refMap;
		}

		struct  Property {
			Property() :type(boolean_type) {}

			enum Type {
				boolean_type,
				integer_type,
				float_type,
				vector2f_type,
				point2f_type,
				point3f_type,
				vector3f_type,
				normal_type,
				spectra_type,
				matrix_type,
				string_type,
				ref_type,
				object_type
			}type;

			struct Value {
				bool boolean_value;
				int integer_value;
				double float_value;
				Vector2f vector2f_value;
				Point2f point2f_value;
				Point3f point3f_value;
				Vector3f vector3f_value;
				Normal3f normal_value;
				Spectrum spectra_value;
				std::string string_value;
			}value;

		};
		int refCount;

		std::map<std::string, Property> propertyMap;
		std::vector<ObjectRef> refQueue;//用于存放直接声明在class内部的对象指针
		std::vector<RefType> refTypeList;
		std::vector<std::string> refIds;

	};
}
#endif