#ifndef _RAVEN_UTILS_PROPERTYLIST_H_
#define _RAVEN_UTILS_PROPERTYLIST_H_

#include"../core/math.h"
#include"../core/spectrum.h"
#include"../core/transform.h"
#include"../core/base.h"
#include<optional>

namespace Raven {

	enum RavenPointerType {
		transform_pointer,
		mapping_pointer,
		shape_pointer,
		texture_float_pointer,
		texture_spectrum_pointer,
		emit_pointer,
		material_pointer,
		primitive_pointer
	};

	struct Pointer {
		Pointer(const std::string id, RavenPointerType p) :pType(p), id(id) {}
		RavenPointerType pType;
		std::string id;
	};

	class PropertyList {
	public:
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

		std::vector<Pointer> pointerList;

		bool getBoolean(const std::string&)const;
		int getInteger(const std::string&)const;
		double getFloat(const std::string&)const;
		Point2f getPoint2f(const std::string&)const;
		Vector2f getVector2f(const std::string&)const;
		Point3f getPoint3f(const std::string&)const;
		Vector3f getVector3f(const std::string&)const;
		Normal3f getNormal3f(const std::string&)const;
		Spectrum getSpectra(const std::string&)const;
		std::string getString(const std::string&)const;

		//void setPointer(const std::string&, RavenPointerType);
		//RavenPointerType getPointer(const std::string&)const;
	private:
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
				string_type
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
		std::map<std::string, Property> propertyMap;
		std::map<std::string, RavenPointerType> pointerMap;
	};
}
#endif