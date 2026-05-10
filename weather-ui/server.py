from http.server import ThreadingHTTPServer, SimpleHTTPRequestHandler
import os

BASE = os.path.dirname(os.path.abspath(__file__))
WEB = os.path.join(BASE, "web")
DATA = os.path.join(BASE, "data")

class Handler(SimpleHTTPRequestHandler):
    def translate_path(self, path):
        if path.startswith("/data/"):
            rel = path[len("/data/"):]
            return os.path.join(DATA, rel)

        rel = path[1:] if path != "/" else "index.html"
        return os.path.join(WEB, rel)

if __name__ == "__main__":
    os.chdir(WEB)
    httpd = ThreadingHTTPServer(("localhost", 8080), Handler)
    print("Open: http://localhost:8080")
    httpd.serve_forever()