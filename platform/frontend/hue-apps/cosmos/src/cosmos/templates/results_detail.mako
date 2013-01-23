<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos - " + title)}

<div class="toolbar">
    <ul class="subnav" data-filters="ArtButtonBar">
        <li>
        ${ title }
        % if page:
            by
            <select id="select-pk" name="select-pk" data-filters="KeySelector">
                <option value="${ primary_key }"
                        selected="selected">${ primary_key }</option>
                % for key, value in page.object_list[0].document.items():
                    % if key not in hidden_keys and key != primary_key:
                    <option value="${ key }">${ key }</option>
                    % endif
                % endfor
            </select>
        % else:
           : no results
        % endif
        </li>

        % if page:
        <li class="pagination">
            <span class="step-links">
                Page ${ page.number } of ${ page.paginator.num_pages } &nbsp;
                % if page.has_previous():
                    <a class="cos-prev_page" title="Previous page"
                        id="link-prev-results" href="?page=${
                        page.previous_page_number() }">previous</a>
                % endif
                % if page.has_next():
                    <a class="cos-next_page" title="Next page"
                       id="link-next-results" href="?page=${
                       page.next_page_number() }">next</a>
                % endif
            </span>
        </li>
        % endif
    </ul>
</div>

<div class="view jframe_padded">
    % if page:
        <table id="job-results-table" data-filters="VisualizedTable">
            <caption class="jframe-hidden">${ title }</caption>
            <thead>
                <tr>
                    <th>${ primary_key } </th>
                    % for key, value in page.object_list[0].document.items():
                        % if (key not in hidden_keys) and (key != primary_key):
                            <th>${ key }</th>
                        % endif
                    % endfor
                </tr>
            </thead>
            <tbody>
            % for job_result in page.object_list:
            <tr>
                <th scope="row">${ job_result.get_primary_key() }</th>
                % for key, value in job_result.get_fields().items():
                    % if key not in hidden_keys:
                        <td class="job-result-value">${ value }</td>
                    % endif
                % endfor
            </tr>
            % endfor
            </tbody>
        </table>
    % else:
        There were no results
    % endif
</div>

${shared.footer()}
## vim:set syntax=mako et:
