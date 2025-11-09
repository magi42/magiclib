///////////////////////////////////////////////////////////////////////////////
// © Copyright Marko Grönroos 1996, all rights reserved.                     //
// Omistusoikeus Marko Grönroos, 1996, kaikki oikeudet pidätetään.           //
//   Tämän tiedoston kopiointi, lukeminen ja käyttö ja erityisesti tämän     //
// ilmoitustekstin muuttaminen on kiellettyä ilman asianmukaista lupaa.      //
//                                                                           //
//                                             Marko Grönroos (magi@utu.fi)  //
///////////////////////////////////////////////////////////////////////////////

// mhtml.h
//- Tukioperaatiot HTML-sivuille.

#ifndef HTML_H
#define HTML_H

#include "magic/mobject.h"
#include "magic/mmap.h"

BEGIN_NAMESPACE (MagiC);

// HTML-kutsu. Näitä objekteja ei sitten mielellään saa luoda enempää
// kuin yhden instanssin, siis erityisesti jos method = POST.
class HTMLQuery : public Object {
	decl_dynamic (HTMLQuery);
  public:

	int		contlen;	// CONTENT_LENGTH
	String	contype;	// CONTENT_TYPE
	String	docroot;	// DOCUMENT_ROOT
	String	gwif;		// GATEWAY_INTERFACE = "cgi/1.1"
	String	hosttype;	// HOSTTYPE = "i386" | ...
	String	httpaccpt;	// HTTP_ACCEPT = "*/*, image/gif, image/jpeg, ..."
	String	httppragma;	// HTTP_PRAGMA = "no-cache" | ...
	String	useragent;	// HTTP_USER_AGENT = "Mozilla xxxx" | ...
	String	pathinfo;	// PATH_INFO
	String	pathtrans;	// PATH_TRANSLATED
	String	querystr;	// QUERY_STRING
	String	remaddr;	// REMOTE_ADDR = "ip.ip.ip.ip"
	String	remhost;	// REMOTE_HOST = "name.address.some.where"
	String	method;		// REQUEST_METHOD = "GET" | "POST"
	String	sptname;	// SCRIPT_NAME
	String	srvname;	// SERVER_NAME = "address.of.local.host"
	int		srvport;	// SERVER_PORT = "80", ...
	String	srvprot;	// SERVER_PROTOCOL = "http/1.0"
	String	srvsoft;	// SERVER_SOFTWARE

	Map<String,String>	query;	// QUERY_STRING pilkotussa muodossa

	// Konstruktori lukee objektin kentät ympäristömuuttujista
					HTMLQuery	();

	// Sokerointi QUERY_STRING-mapin arvojen noudolle
	const String&	operator []	(const String& key) const {return query[key];}

	// Tulostaa kyselyn kenttiä vastaavat piilotetut INPUT-kentät
	void			printhidden	() const;

	// Sivuvaihde. Lukee PATH_INFO-parametrin arvon (esim. "sivu.html"
	// ja kutsuu sen mukaan luokan "sivu_html" metodia printqry. Katso
	// alla HTMLPageBase- ja HTMLPage-luokkia.
	virtual void	any			();

	// Kuten yllä, mutta kutsuu tietyn nimistä sivu-luokka-metodia
	int				call		(const String& pagename);
};


class HTMLPageBase : public Object {
	decl_dynamic (HTMLPageBase);

  public:
					HTMLPageBase	() {;}

	// Hook-operaatio, jonka käyttäjän tulisi overloadata.
	// HTMLPage-makro paketoi tämän kaiken kauniiseen pakettiin
	virtual void	printqry		(HTMLQuery& qry) {
		content ();
		title ("Error");
		printf ("<H1>Error - no print method in page handler</H1>\n");
	}

	// Tulostaa Content-type-otsikon
	void			content			(const char* str="text/html") {
		printf ("Content-type: text/html\n\n");
	}

	// Tulostaa sivun otsikon
	void			title			(const char* str) const {
		printf ("<TITLE>%s</TITLE>\n", str);
	}
};

//- Alleoleva luokka on siis makro, jota kutsutaan:
//- HTMLPage(CLASS,APPCLASS) { ... }
//- Tässä "funktiossa" on parametri 'query', joka on tyyppiä APPCLASS.
//- Tätä "funktiota" kutsutaan kun saadaan kutsu "funktio.html", ja
//- CLASS-parametri on funktio_html.
//- Hieman monimutkaista, eh?

#define HTMLPage(CLASS,APPCLASS) \
class CLASS : public HTMLPageBase {\
	decl_dynamic (CLASS);\
  public:\
  					CLASS		() {;}\
	void			print		(APPCLASS& app);\
	void			printqry	(HTMLQuery& qry) {\
		if (APPCLASS* app = dynamic_cast<APPCLASS*>(&qry))\
			print (*app);\
	}\
};\
impl_dynamic (CLASS, "HTMLPageBase");\
void CLASS::print (APPCLASS& query)


#define ASSERTHTML(expr,message) {if (!(expr)) {cout << "Content-type: text/html\n\n<H3>%ERR: Violation of assertion at " << __FILE__ << "[" << __LINE__ << "].</H3>\n%ERR: " << message << "\n"; exit (2);}}

#define DIEHTML(message) {cout << "Content-type: text/html\n\n<H3>%ERR: Unconditional death at " << __FILE__ << "[" << __LINE__ << "].</H3>\n%ERR: " << message << "\n"; exit (2);}

END_NAMESPACE;

#endif
