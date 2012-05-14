<%namespace name="shared" file="shared_components.mako" />

${shared.header("Cosmos - " + title)}

<div class="view jframe_padded">
    % if job_results:
        <select id="select-pk" name="select-pk" data-filters="KeySelector">
            % for key, value in prototype_result.document.items():
                % if key not in hidden_keys:
                <option 
                        % if key == primary_key:
                        selected="selected"
                        % endif
                        value="${ key }">${ key }
                </option>
                % endif
            % endfor
        </select>
        <table id="job-results-table" data-filters="VisualizedTable">
            <caption>${ title }</caption>
            <thead>
                <tr>
                    <td></td>
                    % for key, value in prototype_result.get_fields().items():
                        % if key not in hidden_keys:
                            <th>${ key }</th>
                        % endif
                    % endfor
                </tr>
            </thead>
            <tbody>
            % for job_result in job_results.object_list:
            <tr>
                <th>${ job_result.get_primary_key() }</th>
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

    ## Pagination
    <div class="pagination">
        <span class="step-links">
            % if job_results.has_previous():
                <a id="link-prev-results" href="?page=${
                    job_results.previous_page_number() }">previous</a>
            % endif

            <span class="current">
                Page ${ job_results.number } of 
                ${ job_results.paginator.num_pages }.
            </span>

            % if job_results.has_next():
                <a id="link-next-results" href="?page=${ 
                    job_results.next_page_number() }">next</a>
            % endif
        </span>
    </div>
</div>

${shared.footer()}
## vim:set syntax=mako:
