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
                    'api/' + $('#client').val() + '/top/' + $('#n').val(),
                    function(data) {
                        $('#categories').html("");
                        $.each(data, function(i, val) {
                            var div = $("<div/>");
                            div.append($('<span class="category"/>').text(val.category))
                               .append($('<span class="count"/>').text(val.count));
                            $('#categories').append(div);
                        })
                    });
            }

            $(document).ready(function() {
                $('#categories').html("--");
                $("#submit").click(do_lookup);
            });
        </script>
    </head>

    <body>
        <h1>API demo</h1>
        <label for="n">Top</label>
        <input name="n" id="n" type="number" value="3" min="1" /> <br/>
        <label for="client">Client</label>
        <select id="client" name="client">
            <option value="ffee16f05e42f3dc">ffee16f05e42f3dc</option>
            <option value="b9980e15c439fe91">b9980e15c439fe91</option>
            <option value="b8ce2743ea776de0">b8ce2743ea776de0</option>
            <option value="b8cf82e1c98c52f7">b8cf82e1c98c52f7</option>
            <option value="b8d01ab20a89bc54">b8d01ab20a89bc54</option>
            <option value="b8d36d1f4d7eb522">b8d36d1f4d7eb522</option>
            <option value="b8d39db3316caa34">b8d39db3316caa34</option>
            <option value="b8d3ef4b5b141df1">b8d3ef4b5b141df1</option>
            <option value="b8d48035ffe4d344">b8d48035ffe4d344</option>
            <option value="b8d494d8967f821a">b8d494d8967f821a</option>
        </select>
        <input type="submit" id="submit" value="Lookup" />

        <p>Categories: <span id="categories">-</span></p>
    </body>
</html>

