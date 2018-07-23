Dynamically injects content into the page. Content can be a block or a
placeholder that can be replaced with the final content.
Any block that is available can be inserted, exemple:
Views, Ads, etc.

# How it works
It provides a context reaction which you can configure the CSS
selector where you want to insert your block.
You can choose if the content should be injected before or after
the element and the distance from top or bottom.

# Requied modules
- Context

# Submodules
- dynamic_inject_block
- dynamic_inject_placeholder
- dynamic_inject_wysiwyg

# Extending
You could literally inject anything you like. Just implement 
hook_dynamic_inject_type_info() and hook_dynamic_inject_render_TYPE()
TYPE means the machine name used in hook_dynamic_inject_type_info().
See the submodules for inspiration.
