#pragma once

class Selectable {
	public:
		virtual int fileno() const = 0;
		virtual ~Selectable() {}
};

void Select(std::list<Selectable*> *rlist,
		std::list<Selectable*> *wlist,
		std::list<Selectable*> *xlist, int ms_timeout = 0);
