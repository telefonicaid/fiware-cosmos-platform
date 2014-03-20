require 'spec_helper'

describe 'recursive_directory' do
    context 'foobar2.py should contain fqdn with default file owner, group, mode' do
        let(:title) { 'example_test' }
        let(:facts) { { :fqdn => 'testhost.localdomain' } }
        let(:params) {
            {   
                :source_dir => 'recursive_directory',
                :dest_dir => '/tmp/dest_dir',
            }
        }

        it do
            should contain_file('/tmp/dest_dir/foobar2.py').with({
                'ensure' => 'file',
                'owner' => 'nobody',
                'group'  => 'nobody',
                'mode'  => '0600',
            }).with_content("testhost.localdomain\n")
        end

    end

    context 'foobar2.py should set proper file mode' do
        let(:title) { 'example_test' }
        let(:facts) { { :fqdn => 'testhost.localdomain' } }
        let(:params) {
            {   
                :source_dir => 'recursive_directory',
                :dest_dir => '/tmp/dest_dir',
                :file_mode => '0711',
            }
        }

        it do
            should contain_file('/tmp/dest_dir/foobar2.py').with({
                'ensure' => 'file',
                'owner' => 'nobody',
                'group'  => 'nobody',
                'mode'  => '0711',
            }).with_content("testhost.localdomain\n")
        end

    end

    context 'foobar2.py should set proper file owner' do
        let(:title) { 'example_test' }
        let(:facts) { { :fqdn => 'testhost.localdomain' } }
        let(:params) {
            {   
                :source_dir => 'recursive_directory',
                :dest_dir => '/tmp/dest_dir',
                :owner => 'root',
            }
        }

        it do
            should contain_file('/tmp/dest_dir/foobar2.py').with({
                'ensure' => 'file',
                'owner' => 'root',
                'group'  => 'nobody',
                'mode'  => '0600',
            }).with_content("testhost.localdomain\n")
        end

    end

    context 'foobar2.py should set proper file group' do
        let(:title) { 'example_test' }
        let(:facts) { { :fqdn => 'testhost.localdomain' } }
        let(:params) {
            {   
                :source_dir => 'recursive_directory',
                :dest_dir => '/tmp/dest_dir',
                :group => 'root',
            }
        }

        it do
            should contain_file('/tmp/dest_dir/foobar2.py').with({
                'ensure' => 'file',
                'owner' => 'nobody',
                'group'  => 'root',
                'mode'  => '0600',
            }).with_content("testhost.localdomain\n")
        end

    end

    context 'foobar2.py should have_custom_fact' do
        let(:title) { 'example_test' }
        let(:facts) { { 
            :fqdn => 'testhost.localdomain',
            :custom_fake_fact => 'this_is_fake'
        } }
        let(:params) {
            {   
                :source_dir => 'recursive_directory',
                :dest_dir => '/tmp/dest_dir',
                :group => 'root',
            }
        }

        it do
            should contain_file('/tmp/dest_dir/custom.conf').with({
                'ensure' => 'file',
                'owner' => 'nobody',
                'group'  => 'root',
                'mode'  => '0600',
            }).with_content("testhost.localdomain\nthis_is_fake\n")
        end

    end

    context 'should have_testdir' do
        let(:title) { 'example_test' }
        let(:facts) { { 
            :fqdn => 'testhost.localdomain',
        } }
        let(:params) {
            {   
                :source_dir => 'recursive_directory',
                :dest_dir => '/tmp/dest_dir',
                :owner => 'root',
                :group => 'root',
            }
        }

        it do
            should contain_file('/tmp/dest_dir/testdir').with({
                'ensure' => 'directory',
                'owner' => 'root',
                'group'  => 'root',
            })
        end

    end

    context 'testdir should have_index.conf file' do
        let(:title) { 'example_test' }
        let(:facts) { { 
            :fqdn => 'testhost.localdomain',
            :custom_fake_fact => 'this_is_fake'
        } }
        let(:params) {
            {   
                :source_dir => 'recursive_directory',
                :dest_dir => '/tmp/dest_dir',
                :owner => 'root',
                :group => 'root',
                :file_mode  => '0600'
            }
        }

        it do
            should contain_file('/tmp/dest_dir/testdir/index.conf').with({
                'ensure' => 'file',
                'owner' => 'root',
                'group'  => 'root',
                'mode'  => '0600',
            }).with_content("testhost.localdomain\n")
        end

    end

    context 'testdir should have_additional_nested folder called nested' do
        let(:title) { 'example_test' }
        let(:facts) { { 
            :fqdn => 'testhost.localdomain',
            :custom_fake_fact => 'this_is_fake'
        } }
        let(:params) {
            {   
                :source_dir => 'recursive_directory',
                :dest_dir => '/tmp/dest_dir',
                :owner => 'root',
                :group => 'root',
                :file_mode  => '0600',
                :dir_mode  => '0755'
            }
        }

        it do
            should contain_file('/tmp/dest_dir/testdir/nested').with({
                'ensure' => 'directory',
                'owner' => 'root',
                'group'  => 'root',
                'mode'   => '0755'
            })
        end

    end
    context 'testdir should have_nested/index.conf file' do
        let(:title) { 'example_test' }
        let(:facts) { { 
            :fqdn => 'testhost.localdomain',
            :custom_fake_fact => 'this_is_fake'
        } }
        let(:params) {
            {   
                :source_dir => 'recursive_directory',
                :dest_dir => '/tmp/dest_dir',
                :owner => 'root',
                :group => 'root',
                :file_mode  => '0600'
            }
        }

        it do
            should contain_file('/tmp/dest_dir/testdir/nested/index.conf').with({
                'ensure' => 'file',
                'owner' => 'root',
                'group'  => 'root',
                'mode'  => '0600',
            }).with_content("testhost.localdomain\n")
        end

    end
end
