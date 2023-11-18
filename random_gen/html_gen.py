# im lazy

import pathlib
import inflect

if __name__ == "__main__":
    engine = inflect.engine()

    for i in range(150, 350, 17):
        word = engine.number_to_words(i)

        dir = pathlib.Path(__file__).parent.resolve()
        file = open(f"{dir}/out/{word}.html", "+w")

        content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>{word}</title>
</head>
<body>
    <h2>{i} in english is {word}</h2>
</body>
</html>
"""

        file.write(content)