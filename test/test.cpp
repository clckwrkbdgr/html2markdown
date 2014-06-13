#include <chthon2/test.h>

int main(int argc, char ** argv)
{
	return Chthon::run_all_tests(argc, argv);
}

TEST(should_pass_text_between_tags)
{
}

TEST(should_remove_extra_whitespaces_in_text_between_tags)
{
}

TEST(should_wrap_p_tag_with_line_breaks)
{
}

TEST(should_remove_extra_whitespaces_in_p_tag)
{
}

TEST(should_wrap_em_tag_with_underscore)
{
}

TEST(should_wrap_i_tag_with_underscore)
{
}

TEST(should_skip_empty_i_tag)
{
}

TEST(should_skip_empty_em_tag)
{
}

TEST(should_wrap_b_tag_with_asterisks)
{
}

TEST(should_wrap_strong_tag_with_asterisks)
{
}

TEST(should_skip_empty_b_tag)
{
}

TEST(should_skip_empty_strong_tag)
{
}

TEST(should_wrap_code_tag_with_backticks)
{
}

TEST(should_skip_empty_code_tag)
{
}

TEST(should_convert_heading_1_to_underscored)
{
}

TEST(should_convert_heading_1_to_hashed)
{
}

TEST(should_convert_heading_2_to_underscored)
{
}

TEST(should_convert_heading_2_to_hashed)
{
}

TEST(should_convert_heading_greater_than_2_up_to_10_to_hashed)
{
}

TEST(should_remove_extra_whitespaces_in_heading_tag)
{
}

TEST(should_convert_hr_tag_to_paragraph_breaker)
{
}

TEST(should_convert_br_tag_to_line_break)
{
}

TEST(should_convert_img_tag_to_markdown_image_element)
{
}

TEST(should_take_alt_text_from_img_tag)
{
}

TEST(should_take_title_text_from_img_tag)
{
}

TEST(should_make_reference_style_link_for_long_img_links)
{
}

TEST(should_remove_extra_whitespaces_in_img_tag)
{
}

TEST(should_convert_a_tag_to_markdown_link_element)
{
}

TEST(should_take_title_text_from_a_tag)
{
}

TEST(should_make_reference_style_link_for_long_a_links)
{
}

TEST(should_remove_extra_whitespaces_in_a_tag)
{
}

TEST(should_take_pre_tag_content_as_it_is_with_tab_indenting)
{
}

TEST(should_take_pre_code_tags_content_as_it_is_with_tab_indenting)
{
}

TEST(should_convert_ol_tag_to_numbered_list)
{
}

TEST(should_convert_ul_tag_to_unnumbered_list)
{
}

TEST(should_skip_li_tag_without_ol_or_ul_tags)
{
}

TEST(should_remove_extra_whitespaces_in_li_tag)
{
}

TEST(should_indent_all_li_content)
{
}

TEST(should_recognize_nested_list)
{
}

TEST(should_prepend_blockquote_content_with_quote_character)
{
}
