#include <chthon2/test.h>
#include "../src/html2mark.h"
using Html2Mark::html2mark;

int main(int argc, char ** argv)
{
	return Chthon::run_all_tests(argc, argv);
}

SUITE(utils) {

TEST(should_collapse_extra_whitespaces)
{
	EQUAL(Html2Mark::collapse("  Text\nwith    whitespaces\t"),
			" Text with whitespaces ");
}

TEST(should_remove_heading_whitespaces)
{
	EQUAL(Html2Mark::collapse("  Text\nwith    whitespaces\t", true),
			"Text with whitespaces ");
}

TEST(should_remove_trailing_whitespaces)
{
	EQUAL(Html2Mark::collapse("  Text\nwith    whitespaces\t", false, true),
			" Text with whitespaces");
}

}

SUITE(html2mark) {

TEST(should_pass_text_between_tags)
{
	EQUAL(html2mark("Text"), "Text");
}

TEST(should_remove_extra_whitespaces_in_text_between_tags)
{
	EQUAL(html2mark("Text\nwith    whitespaces\t"), "Text with whitespaces ");
}

TEST(should_wrap_p_tag_with_line_breaks)
{
	EQUAL(html2mark("<p>Text</p>"), "\nText\n");
	EQUAL(html2mark("<p>Text"), "\nText\n");
	EQUAL(html2mark("<p>Text</p><p>Another</p>"), "\nText\n\nAnother\n");
	EQUAL(html2mark("<p>Text<p>Another"), "\nText\n\nAnother\n");
}

TEST(should_remove_extra_whitespaces_in_p_tag)
{
	EQUAL(html2mark("<p>Text\nwith    whitespaces\t</p>"),
			"\nText with whitespaces\n");
}

TEST(should_wrap_em_tag_with_underscore)
{
	EQUAL(html2mark("<em>Text</em>"), "_Text_");
}

TEST(should_wrap_i_tag_with_underscore)
{
	EQUAL(html2mark("<i>Text</i>"), "_Text_");
}

TEST(should_skip_empty_i_tag)
{
	EQUAL(html2mark("<i></i>"), "");
}

TEST(should_skip_empty_em_tag)
{
	EQUAL(html2mark("<em></em>"), "");
}

TEST(should_wrap_b_tag_with_asterisks)
{
	EQUAL(html2mark("<b>Text</b>"), "**Text**");
}

TEST(should_wrap_strong_tag_with_asterisks)
{
	EQUAL(html2mark("<strong>Text</strong>"), "**Text**");
}

TEST(should_skip_empty_b_tag)
{
	EQUAL(html2mark("<b></b>"), "");
}

TEST(should_skip_empty_strong_tag)
{
	EQUAL(html2mark("<strong></strong>"), "");
}

TEST(should_process_inline_tags_for_text_formatting)
{
	EQUAL(html2mark("Hello, <b><i>world</i></b>"), "Hello, **_world_**");
	EQUAL(html2mark("Hello, <b><i></i></b>world"), "Hello, world");
	EQUAL(html2mark("Hello, <i><b>world</b></i>"), "Hello, _**world**_");
	EQUAL(html2mark("Hello, <i><b>world</i>"), "Hello, _**world**_");
	EQUAL(html2mark("Hello, <i>world</b></i>"), "Hello, _world_");
}

TEST(should_wrap_code_tag_with_backticks)
{
	EQUAL(html2mark("<code>Text</code>"), "`Text`");
}

TEST(should_store_spaces_inside_code_tag_as_they_are)
{
	EQUAL(html2mark("<code>    Some\ttext </code>"), "`    Some\ttext `");
}

TEST(should_skip_empty_code_tag)
{
	EQUAL(html2mark("<code></code>"), "");
}

TEST(should_convert_heading_1_to_underscored)
{
	EQUAL(html2mark("<h1>Text</h1>", Html2Mark::UNDERSCORED_HEADINGS),
			"\nText\n====\n");
}

TEST(should_convert_heading_1_to_hashed)
{
	EQUAL(html2mark("<h1>Text</h1>"), "\n# Text\n");
}

TEST(should_convert_heading_2_to_underscored)
{
	EQUAL(html2mark("<h2>Text</h2>", Html2Mark::UNDERSCORED_HEADINGS),
			"\nText\n----\n");
}

TEST(should_convert_heading_2_to_hashed)
{
	EQUAL(html2mark("<h2>Text</h2>"), "\n## Text\n");
}

TEST(should_convert_heading_greater_than_2_up_to_10_to_hashed)
{
	EQUAL(html2mark("<h3>Text</h3>"),   "\n### Text\n");
	EQUAL(html2mark("<h4>Text</h4>"),   "\n#### Text\n");
	EQUAL(html2mark("<h5>Text</h5>"),   "\n##### Text\n");
	EQUAL(html2mark("<h6>Text</h6>"),   "\n###### Text\n");
}

TEST(should_remove_extra_whitespaces_in_heading_tag)
{
	EQUAL(html2mark("<h1>Text\nwith    whitespaces\t</h1>"),
			"\n# Text with whitespaces\n");
}

TEST(should_convert_hr_tag_to_paragraph_breaker)
{
	EQUAL(html2mark("<hr />"), "\n* * *\n");
	EQUAL(html2mark("<hr/>"), "\n* * *\n");
	EQUAL(html2mark("<hr>"), "\n* * *\n");
}

TEST(should_convert_br_tag_to_line_break)
{
	EQUAL(html2mark("<br />"), "\n");
	EQUAL(html2mark("<br/>"), "\n");
	EQUAL(html2mark("<br>"), "\n");
}

TEST(should_convert_img_tag_to_markdown_image_element)
{
	EQUAL(html2mark("<img src=\"/path/to/img\" />"), "![](/path/to/img)");
}

TEST(should_take_alt_text_from_img_tag)
{
	EQUAL(html2mark("<img src=\"/path/to/img\" alt=\"Alt\" />"),
			"![Alt](/path/to/img)");
}

TEST(should_take_title_text_from_img_tag)
{
	EQUAL(html2mark("<img src=\"/path/to/img\" title=\"Title\" />"),
			"![](/path/to/img \"Title\")");
}

TEST(should_make_reference_style_link_for_long_img_links)
{
	EQUAL(
		html2mark(
			"<img src=\"/a/long/path/to/img\" alt=\"Alt\" title=\"Title\" />",
			Html2Mark::MAKE_REFERENCE_LINKS, 15
			),
		"![Alt][1]\n\n[1]: /a/long/path/to/img \"Title\"\n"
		);
}

TEST(should_convert_a_tag_to_markdown_link_element)
{
	EQUAL(html2mark("<a href=\"http://example.com/\">Text</a>"),
			"[Text](http://example.com/)");
}

TEST(should_take_title_text_from_a_tag)
{
	EQUAL(html2mark("<a href=\"http://example.com/\" title=\"Title\">Text</a>"),
			"[Text](http://example.com/ \"Title\")");
}

TEST(should_make_reference_style_link_for_long_a_links)
{
	EQUAL(
			html2mark(
				"<a href=\"http://example.com\" title=\"Title\">Text</a>",
				Html2Mark::MAKE_REFERENCE_LINKS, 15),
			"[Text][1]\n\n[1]: http://example.com \"Title\"\n"
		 );
}

TEST(should_remove_extra_whitespaces_in_a_tag)
{
	EQUAL(html2mark("<a href=\"http://example.com/\">   \nSome\n\ttext</a>"),
			"[ Some text](http://example.com/)");
}

TEST(should_take_pre_tag_content_as_it_is_with_tab_indenting)
{
	EQUAL(html2mark("<pre>some\n\ttext</pre>"), "\n\tsome\t\ttext\n");
}

TEST(should_take_pre_code_tags_content_as_it_is_with_tab_indenting)
{
	EQUAL(html2mark("<pre><code>some\n\ttext</code></pre>"),
			"\n\tsome\t\ttext\n");
}

TEST(should_convert_ol_tag_to_numbered_list)
{
	EQUAL(html2mark("<ol><li>one</li><li>two<li>three</ol>"),
			"\n1. one\n2. two\n3. three\n");
}

TEST(should_convert_ul_tag_to_unnumbered_list)
{
	EQUAL(html2mark("<ul><li>one</li><li>two<li>three</ul>"),
			"\n* one\n* two\n* three\n");
}

TEST(should_skip_li_tag_without_ol_or_ul_tags)
{
	EQUAL(html2mark("<li>one</li><ul><li>two<li>three</ul>"),
			"one\n\n* two\n* three\n");
}

TEST(should_remove_extra_whitespaces_in_li_tag)
{
	EQUAL(html2mark("<ul><li>\tsome\ntext    </li><li>two<li>three</ul>"),
			"\n* some text\n* two\n* three\n");
}

TEST(should_indent_all_li_content)
{
	EQUAL(html2mark("<ul><li><p>some<p>text</li></ul>"),
			"\n* some\n  text\n\n");
}

TEST(should_recognize_nested_list)
{
	EQUAL(html2mark("<ul><li><ol><li>some<li>text</ol></ul>"),
			"\n* * some\n  * text\n\n");
}

TEST(should_prepend_blockquote_content_with_quote_character)
{
	EQUAL(html2mark("<blockquote><h1>some</h1><p>text</p></blockquote>"),
				"\n> # some\n> text\n");
}

}
