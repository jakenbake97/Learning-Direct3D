/*****************************************************************
 * This is essentially a copy of ChiliException from the DirectX *
 * tutorial series on youtube at: https://youtu.be/QYGLXhulvVQ   *
 *****************************************************************/
#pragma once
#include <exception>
#include <string>

class D3DException : public std::exception
{
public:
	D3DException(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
protected:
	// stores the string from the 'what' function override
	mutable std::string whatBuffer; // mutable so it can be edited in 'what' which is const
private:
	int line;
	std::string file;
};