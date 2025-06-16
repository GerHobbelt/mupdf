
#pragma once

#include <string>

class Parameter {
public:
	void set_name(std::string name);
	void set_value(std::string value);

	std::string name() const;
	std::string value() const;
};

class FeatureFunctionDescriptor
{
public:
	void set_type(std::string type);
	void set_name(std::string name);

	Parameter *add_parameter();
	int parameter_size() const;

	FeatureFunctionDescriptor *add_feature();
	FeatureFunctionDescriptor *mutable_feature(int i);
	int feature_size() const;

	bool has_argument() const;
	int argument() const;

	Parameter& parameter(int i) const;

	std::string name() const;
	std::string type() const;
};

class FeatureExtractorDescriptor {
public:
	FeatureFunctionDescriptor *add_feature();
	FeatureFunctionDescriptor *mutable_feature(int i);
	int feature_size() const;
};

