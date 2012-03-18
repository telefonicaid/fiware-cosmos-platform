<%@page contentType="text/html" pageEncoding="UTF-8"%>
<html>
    <head>
        <title>Word Count API Demo</title>
        <style>
            .category {
                margin-right: 1em;
            }
        </style>

        <script type="text/javascript" src="js/lib/jquery-1.7.1.min.js"></script>
        <script type="text/javascript">
            function do_lookup() {
                $.getJSON(
                    'api/wordcount/' + $('#word').val(),
                    function(data) {
                        $('#count').html(data);
                    });
            }

            $(document).ready(function() {
                $('#count').html("--");
                $("#submit").click(do_lookup);
            });
        </script>
    </head>

    <body>
        <h1>Word Count API Demo</h1>
        <label for="word">Word</label>
        <input type="text" name="word" id="word" />
        <input type="submit" id="submit" value="Lookup" />
        <p>Count: <span id="count"></span></p>
    </body>
</html>
