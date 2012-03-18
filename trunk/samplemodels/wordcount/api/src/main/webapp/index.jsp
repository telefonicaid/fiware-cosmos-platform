<%@page contentType="text/html" pageEncoding="UTF-8"%>
<html>
    <head>
        <title>API demo</title>
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
                        $('#count').html("");
                        $.each(data, function(i, val) {
                            $('#count').append(val);
                        })
                    });
            }

            $(document).ready(function() {
                $('#wordcount').html("--");
                $("#submit").click(do_lookup);
            });
        </script>
    </head>

    <body>
        <h1>API demo</h1>
        <label for="word">Word</label>
        <input type="text" name="word" />
        <input type="submit" id="submit" value="Lookup" />

        <p>Count <span id="count">-</span></p>
    </body>
</html>

