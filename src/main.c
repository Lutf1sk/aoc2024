#include <lt/io.h>
#include <lt/str.h>
#include <lt/sort.h>
#include <lt/math.h>
#include <lt/ctype.h>

#define ilesser(a, b) (a) < (b)
LT_DEFINE_QUICKSORT_FUNC(u64, sort_packed_uq, ilesser)

void day1(lstr_t input_path) {
	void* input_data = NULL;
	usz input_len = 0;
	if LT_UNLIKELY (lt_fmapallp(input_path, &input_data, &input_len)) {
		lt_ferrf("failed to memory map input file '%S'\n", input_path);
	}

	u64 list1[1024];
	u64 list2[1024];

	usz count = 0;
	for (char* it = input_data, *end = it + input_len;; ++count) {
		char* start = it;
		while (it < end && *it != '\n')
			++it;
		lstr_t line = lt_lsfrom_range(start, it++);

		if (!line.len)
			break;
		
		if LT_UNLIKELY (lt_lstou(lt_lstake(line, 5), list1 + count) || lt_lstou(lt_lsdrop(line, 8), list2 + count)) {
			lt_ferrf("failed to parse integer\n");
		}
	}

	sort_packed_uq(count, list1);
	sort_packed_uq(count, list2);

	usz occurences_size = sizeof(u64) * 100000;
	u64* occurences = lt_hmalloc(occurences_size);
	lt_mzero(occurences, occurences_size);

	u64 distance_total = 0;
	for (usz i = 0; i < count; ++i) {
		distance_total += lt_i64_abs(list1[i] - list2[i]);
		++occurences[list2[i]];
	}

	u64 similarity_total = 0;
	for (usz i = 0; i < count; ++i) {
		u64 left = list1[i];
		similarity_total += left * occurences[left];
	}

	lt_printf("total distance: %uq\n", distance_total);
	lt_printf("similarity score: %uq\n", similarity_total);
}

b8 day2_is_safe(u64* ents, usz count, usz mask) {
	usz unmasked0 = 0 + (mask == 0);
	usz unmasked1 = 1 + (mask == 1) + unmasked0;

	u64 prev = ents[unmasked0];
	u64 rising = prev < ents[unmasked1];
	for (usz j = unmasked1; j < count; ++j) {
		if (j == mask) {
			continue;
		}

		u64 current = ents[j];
		u64 delta = lt_i64_abs(current - prev);
		if (delta < 1 || delta > 3 || (prev < current) != rising) {
			return 0;
		}
		prev = current;
	}
	return 1;
}

b8 day2_is_safe_dampened(u64* ents, usz count) {
	if (day2_is_safe(ents, count, -1)) {
		return 1;
	}

	for (usz i = 0; i < count; ++i) {
		if (day2_is_safe(ents, count, i)) {
			return 1;
		}
	}
	return 0;
}

void day2(lstr_t input_path) {
	void* input_data = NULL;
	usz input_len = 0;
	if LT_UNLIKELY (lt_fmapallp(input_path, &input_data, &input_len)) {
		lt_ferrf("failed to memory map input file '%S'\n", input_path);
	}

	u64 entry_counts[1024];
	u64 reports[1024][8];

	usz count = 0;
	for (char* it = input_data, *end = it + input_len;; ++count) {
		char* line_start = it;
		while (it < end && *it != '\n') {
			while (it < end && *it == ' ')
				++it;

			char* num_start = it;
			while (it < end && lt_is_digit(*it))
				++it;

			if LT_UNLIKELY (lt_lstou(lt_lsfrom_range(num_start, it), reports[count] + entry_counts[count]++)) {
				lt_ferrf("failed to parse integer\n");
			}
		}
		if (line_start == it++)
			break;
	}
	
	u64 safe_total = 0;
	u64 safe_dampened = 0;
	for (usz i = 0; i < count; ++i) {
		safe_total += day2_is_safe(reports[i], entry_counts[i], -1);
		safe_dampened += day2_is_safe_dampened(reports[i], entry_counts[i]);
	}
	lt_printf("total safe: %uq\n", safe_total);
	lt_printf("total safe (dampened): %uq\n", safe_dampened);
}

void day3(lstr_t input_path) {
	void* input_data = NULL;
	usz input_len = 0;
	if LT_UNLIKELY (lt_fmapallp(input_path, &input_data, &input_len)) {
		lt_ferrf("failed to memory map input file '%S'\n", input_path);
	}

	u64 total = 0;
	u64 total_enabled = 0;
	u64 enabled_mult = 1;

	for (char* it = input_data, *end = it + input_len; it < end; ++it) {
		u64 n0, n1;
		char* p = it + 4;

		lstr_t remain = lt_lsfrom_range(it, end);
		if (lt_lsprefix(remain, CLSTR("do()"))) {
			enabled_mult = 1;
			continue;
		}
		if (lt_lsprefix(remain, CLSTR("don't()"))) {
			enabled_mult = 0;
			continue;
		}

		if (!lt_lsprefix(remain, CLSTR("mul(")))
			continue;

		char* nstart = p;
		while (p < end && lt_is_digit(*p))
			++p;
		if (p >= end || *p != ',' || p - nstart > 3 || lt_lstou(lt_lsfrom_range(nstart, p), &n0))
			continue;
		++p;

		nstart = p;
		while (p < end && lt_is_digit(*p))
			++p;
		if (p >= end || *p != ')' || p - nstart > 3 || lt_lstou(lt_lsfrom_range(nstart, p), &n1))
			continue;

		total += n0 * n1;
		total_enabled += n0 * n1 * enabled_mult;
	}

	lt_printf("total: %uq\n", total);
	lt_printf("total (enabled): %uq\n", total_enabled);
}

void day4(lstr_t input_path) {
	void* input_data = NULL;
	usz input_len = 0;
	if LT_UNLIKELY (lt_fmapallp(input_path, &input_data, &input_len)) {
		lt_ferrf("failed to memory map input file '%S'\n", input_path);
	}

	usz w = 141, h = 140;
	usz occurence_total = 0;
	usz x_mas_occurences = 0;

	char* pline = input_data;
	for (usz i = 0; i < h; ++i) {
		for (char* it = pline, *line_end = it + w - 1; it < line_end; ++it) {
			usz wrem = line_end - it;
			usz hrem = h - i;

			if (wrem >= 4 && (!memcmp(it, "XMAS", 4) || !memcmp(it, "SAMX", 4)))
				++occurence_total;
			if (hrem >= 4 && ((it[w*0] == 'X' && it[w*1] == 'M' && it[w*2] == 'A' && it[w*3] == 'S') || (it[w*0] == 'S' && it[w*1] == 'A' && it[w*2] == 'M' && it[w*3] == 'X')))
				++occurence_total;
			if (wrem >= 4 && hrem >= 4 && ((it[w*0+0] == 'X' && it[w*1+1] == 'M' && it[w*2+2] == 'A' && it[w*3+3] == 'S') || (it[w*0+0] == 'S' && it[w*1+1] == 'A' && it[w*2+2] == 'M' && it[w*3+3] == 'X')))
				++occurence_total;
			if ((it - pline) >= 3 && hrem >= 4 && ((it[w*0-0] == 'X' && it[w*1-1] == 'M' && it[w*2-2] == 'A' && it[w*3-3] == 'S') || (it[w*0-0] == 'S' && it[w*1-1] == 'A' && it[w*2-2] == 'M' && it[w*3-3] == 'X')))
				++occurence_total;

			if (wrem < 3 || hrem < 3)
				continue;

			if (
				((it[w*0+0] == 'M' && it[w*1+1] == 'A' && it[w*2+2] == 'S') ||
				 (it[w*0+0] == 'S' && it[w*1+1] == 'A' && it[w*2+2] == 'M')) &&
				((it[w*2+0] == 'M' && it[w*1+1] == 'A' && it[w*0+2] == 'S') ||
				 (it[w*2+0] == 'S' && it[w*1+1] == 'A' && it[w*0+2] == 'M')))
			{
				++x_mas_occurences;
			}

		}

		pline += w;
	}

	lt_printf("total: %uz\n", occurence_total);
	lt_printf("total (x-mas): %uz\n", x_mas_occurences);
}

void day5(lstr_t input_path) {
	void* input_data = NULL;
	usz input_len = 0;
	if LT_UNLIKELY (lt_fmapallp(input_path, &input_data, &input_len)) {
		lt_ferrf("failed to memory map input file '%S'\n", input_path);
	}
	
	usz rule_count = 0;
	u8 rules[4096][2];

	char* it = input_data, *end = it + input_len;
	while (it < end && *it != '\n') {
		u64 r0, r1;
		if LT_UNLIKELY (lt_lstou(LSTR(it, 2), &r0) || lt_lstou(LSTR(it + 3, 2), &r1))
			lt_ferrf("failed to parse integer\n");
		rules[rule_count][0] = r0;
		rules[rule_count++][1] = r1;
		it += 6;
	}
	++it;

	usz total = 0;
	usz total_fixed = 0;
	while (it < end && *it != '\n') {
		usz page_count = 0;
		u64 pages[128];

		while (it < end) {
			if LT_UNLIKELY (lt_lstou(LSTR(it, 2), pages + page_count++))
				lt_ferrf("failed to parse integer\n");
			it += 2;
			if (*it++ == '\n')
				break;
		}

		for (usz i = 0; i < page_count; ++i) {
			for (usz j = i + 1; j < page_count; ++j) {
				for (usz k = 0; k < rule_count; ++k) {
					if (rules[k][1] == pages[i] && rules[k][0] == pages[j]) {
						goto invalid;
					}
				}
			}
		}
		total += pages[page_count >> 1];
		continue;

	invalid:
		for (usz i = 0; i < page_count; ++i) {
			for (usz j = i + 1; j < page_count; ++j) {
				for (usz k = 0; k < rule_count; ++k) {
					if (rules[k][1] == pages[i] && rules[k][0] == pages[j]) {
						u8 p = pages[i];
						pages[i] = pages[j];
						pages[j] = p;
						--k;
						break;
					}
				}
			}
		}

		total_fixed += pages[page_count >> 1];
	}
	lt_printf("total: %uq\n", total);
	lt_printf("total (fixed): %uq\n", total_fixed);
}

void day6(lstr_t input_path) {
	void* input_data = NULL;
	usz input_len = 0;
	if LT_UNLIKELY (lt_fmapallp(input_path, &input_data, &input_len)) {
		lt_ferrf("failed to memory map input file '%S'\n", input_path);
	}

	usz w = 131, h = 130;
	char* map = input_data;
	b8 walked[130][130];
	lt_mzero(walked, sizeof(walked));

	usz guard_x = -1, guard_y = -1;
	for (usz x = 0; x < w - 1; ++x) {
		for (usz y = 0; y < h; ++y) {
			if (map[y*w+x] == '^') {
				guard_x = x;
				guard_y = y;
				goto guard_found;
			}
		}
	}
guard_found:;

	u8 dir = 0;
	i8 xadv_tab[4] = { 0, 1, 0, -1 };
	i8 yadv_tab[4] = { -1, 0, 1, 0 };

	walked[guard_y][guard_x] = 1;
	do {
		usz next_x, next_y;
		for (;;) {
			next_x = guard_x + xadv_tab[dir];
			next_y = guard_y + yadv_tab[dir];
			if (map[next_y*w + next_x] != '#')
				break;
			dir = (dir + 1) & 0x03;
		}
		guard_x = next_x;
		guard_y = next_y;
		walked[guard_y][guard_x] = 1;
	} while (guard_x > 0 && guard_x < w - 1 && guard_y > 0 && guard_y < h);

	usz total_walked = 0;
	for (b8* it = (b8*)walked, *end = it + sizeof(walked); it < end; ++it) {
		total_walked += *it;
	}

	lt_printf("%uz\n", total_walked);
}

int main(int argc, char** argv) {
	//day1(CLSTR("input1.txt"));
	//day2(CLSTR("input2.txt"));
	//day3(CLSTR("input3.txt"));
	//day4(CLSTR("input4.txt"));
	//day5(CLSTR("input5.txt"));
	day6(CLSTR("input6.txt"));
	return 0;
}

