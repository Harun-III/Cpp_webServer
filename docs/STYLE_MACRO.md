# WebServ Style Macro

## Overview
The `WEBSERV_STYLE` macro provides a consistent Matrix-themed style across all server pages (directory listings, error pages, etc.).

## Usage

### 1. Include the header
```cpp
#include "WebservStyle.hpp"
```

### 2. Use the macro in your HTML generation
```cpp
ss << "<!DOCTYPE html><html lang=\"en\"><head>"
   << "<meta charset=\"UTF-8\">"
   << "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
   << "<title>Your Page Title</title>"
   << WEBSERV_STYLE  // Insert the style macro here
   << "<div class=\"container\">"
   << "<h1>Your Heading</h1><hr>"
   << "<!-- Your content -->"
   << "</div></body></html>";
```

## Available CSS Classes

### `.container`
- Max-width: 1000px
- Centered with auto margins
- 24px padding

### `.list`
- Grid layout with 12px gap
- For directory listings

### `.item a`
- Inline-block links
- Green border with gradient background
- Hover effect: inverts to green background with black text

### `h1`
- Green glow text-shadow effect
- Bold font weight

### `hr`
- Transparent border with green top line
- Minimal spacing

## Theme Colors
- Primary: `#00ff41` (Matrix green)
- Background gradient: Dark gradient from `#0a0a0a` to `#000000`
- Font: 'Fira Code' (imported from Google Fonts)

## Example Pages Using This Macro
- Directory listing (auto-index)
- Error pages (can be updated to use this)
- Any custom HTML responses

## Benefits
- **Consistency**: All pages share the same look and feel
- **Maintainability**: Update style in one place
- **Minimal code**: Reduces duplication across files
- **Performance**: No external CSS file needed
