#pragma once

class Selectable {
	public:
		virtual int fileno() = 0;
		virtual ~Selectable() {}
};
