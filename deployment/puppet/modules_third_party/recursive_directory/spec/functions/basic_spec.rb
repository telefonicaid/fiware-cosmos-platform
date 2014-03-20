require 'spec_helper'

describe 'recurse_directory' do
    # Test that providing all params
    # returns them back
    it { should run.with_params(
                'test',
                'dest_dir',
                '0777',
                'file_owner',
                'file_group'
            ).and_return({
        "dest_dir/example.conf" =>
            {
                "ensure" => "file",
                "content" => "\n",
                "owner"   => 'file_owner',
                "group"   => 'file_group',
                "mode"   => '0777'
            }
    }) }

    # Test that omitting owner, group, mode
    # returns the proper defaults
    it { should run.with_params(
                'test',
                'dest_dir'
            ).and_return({
        "dest_dir/example.conf" =>
            {
                "ensure" => "file",
                "content" => "\n",
                "mode"   => '0600'
            }
    }) }
end

