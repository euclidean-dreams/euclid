import os

from flask import Flask

app = Flask(__name__)

@app.after_request
def add_header(response):
    response.headers["Cross-Origin-Opener-Policy"] = "same-origin"
    response.headers["Cross-Origin-Embedder-Policy"] = "require-corp"
    return response

if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0", port=8080)
