#include <chthon2/test.h>
#include <chthon2/log.h>
#include "../src/html2mark.h"
using Html2Mark::html2mark;

int main(int argc, char ** argv)
{
	return Chthon::run_all_tests(argc, argv);
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
	EQUAL(html2mark("<p>one<br>\ntwo<br>\nthree<br>\nfour<br>\nfive</p>\n"),
			"\none\ntwo\nthree\nfour\nfive\n"
		 );
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
			"[Some text](http://example.com/)");
}

TEST(should_take_pre_tag_content_as_it_is_with_tab_indenting)
{
	EQUAL(html2mark("<pre>some\n\ttext</pre>"), "\n\tsome\n\t\ttext\n");
}

TEST(should_take_pre_code_tags_content_as_it_is_with_tab_indenting)
{
	EQUAL(html2mark("<pre><code>some\n\ttext</code></pre>"),
			"\n\tsome\n\t\ttext\n");
	EQUAL(html2mark("<p><pre><code>some\n\ttext</code></pre></p>"),
			"\n\n\tsome\n\t\ttext\n");
}

TEST(should_convert_ol_tag_to_numbered_list)
{
	EQUAL(html2mark("<ol><li>one</li><li>two<li>three</ol>"),
			"\n1. one\n2. two\n3. three\n");
}

TEST(should_pass_p_tag_inside_li)
{
	EQUAL(html2mark("<ol><li><p>one</p></li></ol>"),
			"\n1. one\n");
}

TEST(should_convert_ul_tag_to_unnumbered_list)
{
	EQUAL(html2mark("<ul><li>one</li><li>two<li>three</ul>"),
			"\n* one\n* two\n* three\n");
}

TEST(should_skip_li_tag_without_ol_or_ul_tags)
{
	EQUAL(html2mark("<li>one</li><ul><li>two<li>three</ul>"),
			"\none\n\n* two\n* three\n");
}

TEST(should_remove_extra_whitespaces_in_li_tag)
{
	EQUAL(html2mark("<ul><li>\tsome\ntext    </li><li>two<li>three</ul>"),
			"\n* some text\n* two\n* three\n");
}

TEST(should_indent_all_li_content)
{
	EQUAL(html2mark("<ul><li><p>some<p>text</li></ul>"),
			"\n* some\n  \n  text\n");
	EQUAL(html2mark("<ul><li>some<br>text</li></ul>"),
			"\n* some\n  text\n");
}

TEST(should_recognize_nested_list)
{
	EQUAL(html2mark("<ul><li><ol><li>some<li>text</ol></ul>"),
			"\n* 1. some\n  2. text\n");
	EQUAL(html2mark("<ol><li><ul><li>some<li>text</ol></ul>"),
			"\n1. * some\n  * text\n");
}

TEST(should_prepend_blockquote_content_with_quote_character)
{
	EQUAL(html2mark("<blockquote><h1>some</h1><p>text</p></blockquote>"),
				"\n> \n> # some\n> \n> text\n");
}

TEST(should_pass_main_html_tags)
{
	EQUAL(html2mark("<html>Some text <b>with bold <i>and italic</i></b></html>"),
				"Some text **with bold _and italic_**");
}

TEST(should_pass_body_tags)
{
	EQUAL(html2mark(
				"<html>"
				"  <body>"
				"    Some text <b>with bold <i>and italic</i></b>"
				"  </body>"
				"</html>"
				),
			"Some text **with bold _and italic_**");
}

TEST(should_skip_head_tag)
{
	EQUAL(html2mark(
				"<html>"
				"  <head>"
				"    <style>html body { background-color: #111; }</style>"
				"  </head>"
				"  <body>"
				"    Some text <b>with bold <i>and italic</i></b>"
				"  </body>"
				"</html>"
				),
			"Some text **with bold _and italic_**");
}

TEST(should_treat_div_tags_as_paragraphs)
{
	EQUAL(html2mark("<div>Some text <b>with bold <i>and italic</i></b></div>"),
				"\nSome text **with bold _and italic_**\n");
}

TEST(should_pass_span_tags)
{
	EQUAL(html2mark(
				"<div>Some <span>text</span> "
				"<b>with bold <i>and italic</i></b></div>"
				),
			"\nSome text **with bold _and italic_**\n");
}

TEST(should_convert_html_entities)
{
	EQUAL(html2mark("&quot;Hello&quot;"), "\"Hello\"");
	EQUAL(html2mark("&nbsp;"), " ");
	EQUAL(html2mark("Hello&nbsp;&nbsp;world"), "Hello  world");
	EQUAL(html2mark("&#171;"), "«");
	EQUAL(html2mark("&#187;"), "»");
	EQUAL(html2mark("&"), "&");
	EQUAL(html2mark("Some &text"), "Some &text");
	EQUAL(html2mark("&#39;"), "'");
	EQUAL(html2mark("&lt;&gt;"), "<>");
	EQUAL(html2mark("&amp; &rarr; &mdash; &#40; &#41; &#58; "), "& → — ( ) : ");
}

TEST(should_collapse_empty_lines)
{
	std::string data =
		"<html>\n"
		"<head>\n"
		"<meta http-equiv=Content-Type content=\"text/html; charset=utf-8\"/>\n"
		"<style type=\"text/css\">\n"
		"	html body { background-color: #111 }\n"
		"	body { color: #bbb }\n"
		"	a { color:#b91 }\n"
		"</style>\n"
		"<title>Lorem ipsum</title>\n"
		"</head>\n"
		"<body>\n"
		"<h1>Lorem ipsum</h1>\n"
		"<p><a href=\"http://www.example.com/data/123456\">http://www.example.com/data/123456</a></p>\n"
		"<p>2014-06-26T20:07:53-04:00</p>\n"
		"<div><p>Lorem ipsum dolor sit amet, consectetur adipisicing elit,<br/>\n"
		"sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.</p></div>\n"
		"</body>\n"
		"</html>\n"
		;
	std::string expected =
		"Lorem ipsum\n"
		"===========\n"
		"\n"
		"[http://www.example.com/data/123456][1]\n"
		"\n"
		"2014-06-26T20:07:53-04:00\n"
		"\n"
		"Lorem ipsum dolor sit amet, consectetur adipisicing elit,\nsed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\n"
		"\n"
		"[1]: http://www.example.com/data/123456\n"
		;
	std::string result = html2mark(
			data,
			Html2Mark::UNDERSCORED_HEADINGS | Html2Mark::MAKE_REFERENCE_LINKS
			);
	EQUAL(result, expected);

}

}

SUITE(colors) {

TEST(should_place_reset_markers_in_the_beginning_and_the_end)
{
	EQUAL(html2mark("Text", Html2Mark::COLORS), "[0mText[0m");
}

TEST(should_mark_emphasized_text_with_cyan)
{
	EQUAL(html2mark("<em>Text</em>", Html2Mark::COLORS), "[0m[00;36mText[0m");
	EQUAL(html2mark("<i>Text</i>", Html2Mark::COLORS), "[0m[00;36mText[0m");
}

TEST(should_mark_strong_text_with_bold_style)
{
	EQUAL(html2mark("<strong>Text</strong>", Html2Mark::COLORS), "[0m[01;37mText[0m");
	EQUAL(html2mark("<b>Text</b>", Html2Mark::COLORS), "[0m[01;37mText[0m");
}

TEST(should_mark_emphasized_strong_text_with_bold_cyan)
{
	EQUAL(html2mark("<b>Hello, <i>world</i></b>", Html2Mark::COLORS),
			"[0m[01;37mHello, [01;36mworld[0m");
	EQUAL(html2mark("<b><i>Hello</i>, world</b>", Html2Mark::COLORS),
			"[0m[01;36mHello[01;37m, world[0m");
}

TEST(should_mark_headers_with_purple)
{
	EQUAL(html2mark("<h1>Text</h1>", Html2Mark::COLORS),
			"[0m[00;35m# Text[0m");
	EQUAL(html2mark("<h1>Text</h1>",
				Html2Mark::COLORS | Html2Mark::UNDERSCORED_HEADINGS),
			"[0m[00;35mText\n====[0m");
}

TEST(should_mark_rulers_with_purple)
{
	EQUAL(html2mark("<hr>", Html2Mark::COLORS),
			"[0m[00;35m* * *[0m");
}

TEST(should_mark_urls_with_blue)
{
	EQUAL(html2mark("<img src=\"/path/to/img\" />", Html2Mark::COLORS),
			"[0m![]([00;34m/path/to/img[0m)[0m");
	EQUAL(html2mark("<a href=\"http://example.com/\">Text</a>", Html2Mark::COLORS),
			"[0m[Text]([00;34mhttp://example.com/[0m)[0m");
	EQUAL(
		html2mark(
			"<img src=\"/a/long/path/to/img\"/>",
			Html2Mark::COLORS | Html2Mark::MAKE_REFERENCE_LINKS, 15
			),
		"[0m![][1]\n\n[1]: [00;34m/a/long/path/to/img[0m"
		);
}

TEST(should_mark_list_bullets_with_yellow)
{
	EQUAL(html2mark("<ol><li>one</li><li>two<li>three</ol>", Html2Mark::COLORS),
			"[0m\n[00;33m1.[0m one\n"
			"[00;33m2.[0m two\n"
			"[00;33m3.[0m three\n[0m");
	EQUAL(html2mark("<ul><li>one</li><li>two<li>three</ul>", Html2Mark::COLORS),
			"[0m\n[00;33m*[0m one\n"
			"[00;33m*[0m two\n"
			"[00;33m*[0m three\n[0m");
}

TEST(should_mark_blockquote_line_with_yellow)
{
	EQUAL(html2mark("<blockquote><h1>some</h1><p>text</p></blockquote>", Html2Mark::COLORS),
				"[0m\n"
				"[00;33m>[0m \n"
				"[00;33m>[0m [00;35m# some[0m\n"
				"[00;33m>[0m \n"
				"[00;33m>[0m text\n[0m");
}

}
