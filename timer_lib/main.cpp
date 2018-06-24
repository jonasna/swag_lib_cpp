#include "timer.h"
#include <string>
#include <iostream>
#include <ctime>


static void print_dope()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::cout << "Dope from timer thread at " << std::ctime(&t);
}

class foo
{
public:
	void doo(std::string derp)
	{
		std::cout << derp << std::endl;
	}
};

int main()
{
	foo fo;

	auto lol = std::bind(&foo::doo, &fo);

	timer my_timer(timer::duration(1000), true);

	auto dope = std::bind(&foo::doo, &fo, "Sup");

	auto id = my_timer.attach_handler(dope);

	my_timer.attach_handler([]()
	{
		std::cout << "Hello from lambda handler" << std::endl;
	});


	my_timer.attach_handler([]()
	{
		std::cout << "Hello from lambda handler" << std::endl;
	});


	my_timer.attach_handler([]()
	{
		std::cout << "Hello from lambda handler" << std::endl;
	});

	my_timer.attach_handler([]()
	{
		std::cout << "Hello from lambda handler" << std::endl;
	});


	my_timer.attach_handler([]()
	{
		std::cout << "Hello from lambda handler" << std::endl;
	});


	my_timer.attach_handler([]()
	{
		std::cout << "Hello from lambda handler" << std::endl;
	});

	my_timer.attach_handler([]()
	{
		std::cout << "Hello from lambda handler" << std::endl;
	});


	my_timer.attach_handler([]()
	{
		std::cout << "Hello from lambda handler" << std::endl;
	});


	my_timer.attach_handler([]()
	{
		std::cout << "Hello from lambda handler" << std::endl;
	});

	my_timer.enable();

	std::this_thread::sleep_for(std::chrono::seconds(5));

	my_timer.disable();


	std::cout << "=====================" << std::endl;
	my_timer.enable();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	my_timer.clear();


	std::this_thread::sleep_for(std::chrono::seconds(3));


	return 0;

}