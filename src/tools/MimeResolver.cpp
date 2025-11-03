# include "MimeResolver.hpp"

MimeResolver::MimeResolver( void ) { initializeMaps(); }

MimeResolver::~MimeResolver( void ) { ; }

std::string	MimeResolver::to_lower( const std::string &string ) const {
	size_t					index;
	std::string				lower_str = string;

	for (index = 0; index < string.size(); ++index) {
		lower_str[index] = static_cast<char>(
			std::tolower(static_cast<unsigned char>(string[index])));
	}

	return lower_str;
}

const std::string	&MimeResolver::getMimeType( const std::string &extension ) const {
	map_s::const_iterator	mimeIter = extToMime.find(to_lower(extension));

	if (mimeIter == extToMime.end()) mimeIter = extToMime.find("bin");
	return mimeIter->second;
}

const std::string	&MimeResolver::getExtension( const std::string &mimeType ) const {
	map_s::const_iterator	extIter = mimeToExt.find(to_lower(mimeType));

	if (extIter == mimeToExt.end())
		extIter = mimeToExt.find("application/octet-stream");

	return extIter->second;
}

void MimeResolver::initializeMaps( void ) {

	// --- TEXT & CODE FILES (text/*) ---
	// Note: 'txt' is preferred for text/plain
	INSERT_MAPPING("txt",		"text/plain");
	INSERT_MAPPING("log",		"text/plain");
	INSERT_MAPPING("c",			"text/plain");
	INSERT_MAPPING("cpp",		"text/plain");
	INSERT_MAPPING("h",			"text/plain");
	INSERT_MAPPING("hpp",		"text/plain");
	
	// Note: 'html' is preferred for text/html
	INSERT_MAPPING("html",		"text/html");
	INSERT_MAPPING("htm",		"text/html");
	
	INSERT_MAPPING("css",		"text/css");
	INSERT_MAPPING("xml",		"text/xml");
	INSERT_MAPPING("csv",		"text/csv");
	INSERT_MAPPING("tsv",		"text/tab-separated-values");
	INSERT_MAPPING("md",		"text/markdown");
	INSERT_MAPPING("rtf",		"application/rtf");


	// --- IMAGE FILES (image/*) ---
	INSERT_MAPPING("png",		"image/png");
	
	// Note: 'jpg' is preferred for image/jpeg
	INSERT_MAPPING("jpg",		"image/jpeg");
	INSERT_MAPPING("jpeg",		"image/jpeg");
	INSERT_MAPPING("jfif",		"image/jpeg");
	
	INSERT_MAPPING("gif",		"image/gif");
	INSERT_MAPPING("bmp",		"image/bmp");
	INSERT_MAPPING("ico",		"image/x-icon");
	
	// Note: 'svg' is preferred for image/svg+xml
	INSERT_MAPPING("svg",		"image/svg+xml");
	INSERT_MAPPING("svgz",		"image/svg+xml");
	
	INSERT_MAPPING("webp",		"image/webp");
	INSERT_MAPPING("tif",		"image/tiff");
	INSERT_MAPPING("tiff",		"image/tiff");
	INSERT_MAPPING("avif",		"image/avif");


	// --- AUDIO FILES (audio/*) ---
	INSERT_MAPPING("mp3",		"audio/mpeg");
	INSERT_MAPPING("wav",		"audio/wav");
	INSERT_MAPPING("ogg",		"audio/ogg");
	INSERT_MAPPING("m4a",		"audio/mp4");
	INSERT_MAPPING("weba",		"audio/webm");


	// --- VIDEO FILES (video/*) ---
	// Note: 'mp4' is preferred for video/mp4
	INSERT_MAPPING("mp4",		"video/mp4");
	INSERT_MAPPING("m4v",		"video/mp4");
	
	INSERT_MAPPING("mov",		"video/quicktime");
	INSERT_MAPPING("webm",		"video/webm");
	INSERT_MAPPING("ogv",		"video/ogg");
	INSERT_MAPPING("avi",		"video/x-msvideo");
	INSERT_MAPPING("wmv",		"video/x-ms-wmv");
	INSERT_MAPPING("3gp",		"video/3gpp");
	INSERT_MAPPING("mkv",		"video/x-matroska");


	// --- FONT FILES (font/*) ---
	INSERT_MAPPING("otf",		"font/otf");
	INSERT_MAPPING("ttf",		"font/ttf");
	INSERT_MAPPING("woff",		"font/woff");
	INSERT_MAPPING("woff2",		"font/woff2");


	// --- APPLICATION FILES (application/*) ---
	// JavaScript
	INSERT_MAPPING("js",		"application/javascript");
	INSERT_MAPPING("mjs",		"application/javascript");
	
	// JSON
	INSERT_MAPPING("json",		"application/json");
	INSERT_MAPPING("jsonld",	"application/ld+json");
	INSERT_MAPPING("gltf",		"model/gltf+json");
	
	// Documents & Data
	INSERT_MAPPING("pdf",		"application/pdf");
	INSERT_MAPPING("wasm",		"application/wasm");
	INSERT_MAPPING("bin",		"application/octet-stream");
	INSERT_MAPPING("dat",		"application/octet-stream");

	// Execution/Scripting
	INSERT_MAPPING("sh",		"application/x-sh");
	INSERT_MAPPING("php",		"application/x-httpd-php");
	INSERT_MAPPING("swf",		"application/x-shockwave-flash");
	INSERT_MAPPING("ps",		"application/postscript");
	INSERT_MAPPING("ai",		"application/postscript");
	INSERT_MAPPING("eps",		"application/postscript");

	// Microsoft Office Formats (DOCX, XLSX, PPTX are preferred)
	INSERT_MAPPING("docx",		"application/vnd.openxmlformats-officedocument.wordprocessingml.document");
	INSERT_MAPPING("doc",		"application/msword");
	INSERT_MAPPING("xlsx",		"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
	INSERT_MAPPING("xls",		"application/vnd.ms-excel");
	INSERT_MAPPING("pptx",		"application/vnd.openxmlformats-officedocument.presentationml.presentation");
	INSERT_MAPPING("ppt",		"application/vnd.ms-powerpoint");


	// --- ARCHIVE & COMPRESSED FILES ---
	INSERT_MAPPING("zip",		"application/zip");
	INSERT_MAPPING("gz",		"application/gzip");
	INSERT_MAPPING("tar",		"application/x-tar");
	INSERT_MAPPING("bz2",		"application/x-bzip2");
	INSERT_MAPPING("xz",		"application/x-xz");
	INSERT_MAPPING("7z",		"application/x-7z-compressed");
	INSERT_MAPPING("rar",		"application/vnd.rar");
	INSERT_MAPPING("deb",		"application/vnd.debian.binary-package");
	INSERT_MAPPING("iso",		"application/x-iso9660-image");
	INSERT_MAPPING("dmg",		"application/x-apple-diskimage");
}
