import os

from flask import Flask, render_template

app = Flask(__name__)


@app.route('/')
def hello():
    return render_template("index.html")


@app.after_request
def add_header(response):
    response.headers["Cross-Origin-Opener-Policy"] = "same-origin"
    response.headers["Cross-Origin-Embedder-Policy"] = "require-corp"
    return response


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=os.getenv("PORT"))
