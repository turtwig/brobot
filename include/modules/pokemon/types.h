#ifndef POKEMON_TYPES_H_INCLUDED
#define POKEMON_TYPES_H_INCLUDED

#include <limits>

namespace pokemon {
	enum Type { none, normal, fight, flying, poison, ground, rock, bug, ghost, steel, fire, water, grass, electric, psychic, ice, dragon, dark };

	namespace types_internal {
		#define H(x, y)  if (a == x) return y;
		inline float t(const Type& a, const Type& d) {
			switch (d) {
				case none:
					return 1.0;
				case normal:
					H(fight, 2.0) H(ghost, 0.0) break;
				case fight:
					H(flying, 2.0) H(rock, 0.5) H(bug, 0.5) H(psychic, 2.0) H(dark, 0.5) break;
				case flying:
					H(fight, 0.5) H(ground, 0.0) H(rock, 2.0) H(bug, 0.5) H(grass, 0.5) H(electric, 2.0) H(ice, 2.0) break;
				case poison:
					H(fight, 0.5) H(poison, 0.5) H(ground, 2.0) H(bug, 0.5) H(grass, 0.5) H(psychic, 2.0) break;
				case ground:
					H(poison, 0.5) H(rock, 0.5) H(water, 2.0) H(grass, 2.0) H(electric, 0.0) H(ice, 2.0) break;
				case rock:
					H(normal, 0.5) H(fight, 2.0) H(flying, 0.5) H(poison, 0.5) H(ground, 2.0) H(steel, 2.0) H(fire, 0.5)H(water, 2.0) H(grass, 2.0) break;
				case bug:
					H(fight, 0.5) H(flying, 2.0) H(ground, 0.5) H(rock, 2.0) H(fire, 2.0) H(grass, 0.5) break;
				case ghost:
					H(normal, 0.0) H(fight, 0.0) H(poison, 0.5) H(bug, 0.5) H(ghost, 2.0) H(dark, 2.0) break;
				case steel:
					H(normal, 0.5) H(fight, 2.0) H(flying, 0.5) H(poison, 0.0) H(ground, 2.0) H(rock, 0.5) H(bug, 0.5) H(ghost, 0.5) H(steel, 0.5) H(fire, 2.0) H(grass, 0.5) H(psychic, 0.5) H(ice, 0.5) H(dragon, 0.5) H(dark, 0.5) break;
				case fire:
					H(ground, 2.0) H(rock, 2.0) H(bug, 0.5) H(steel, 0.5) H(fire, 0.5) H(water, 2.0) H(grass, 0.5) H(ice, 0.5) break;
				case water:
					H(steel, 0.5) H(fire, 0.5) H(water, 0.5) H(grass, 2.0) H(electric, 2.0) H(ice, 0.5) break;
				case grass:
					H(flying, 2.0) H(poison, 2.0) H(ground, 0.5) H(bug, 2.0) H(fire, 2.0) H(water, 0.5) H(grass, 0.5) H(electric, 0.5) H(ice, 2.0) break;
				case electric:
					H(flying, 0.5) H(ground, 2.0) H(steel, 0.5) H(electric, 0.5) break;
				case psychic:
					H(fight, 0.5) H(bug, 2.0) H(ghost, 2.0) H(psychic, 0.5) H(dark, 2.0) break;
				case ice:
					H(fight, 2.0) H(rock, 2.0) H(steel, 2.0) H(fire, 2.0) H(ice, 0.5) break;
				case dragon:
					H(fire, 0.5) H(water, 0.5) H(grass, 0.5) H(electric, 0.5) H(ice, 2.0) H(dragon, 2.0) break;
				case dark:
					H(fight, 2.0) H(bug, 2.0) H(ghost, 0.5) H(psychic, 0.0) H(dark, 0.5) break;
			}
			return 1.0;
		}
		#undef H
	}

	float TypeModifier(const Type& a, const Type& def1, const Type& def2) {
		if (a == none || (def1 == none && def2 == none))
			return std::numeric_limits<float>::quiet_NaN();
		return types_internal::t(a, def1) * types_internal::t(a, def2);
	};

	inline float SameTypeBonus(const Type& a, const Type& t1, const Type& t2) {
		if (a == none || (t1 == none && t2 == none))
			return std::numeric_limits<float>::quiet_NaN();
		if (t1 == a || t2 == a) {
			return 1.5;
		} else {
			return 1.0;
		}
	};
}

#endif //POKEMON_TYPES_H_INCLUDED