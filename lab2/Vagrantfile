Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/focal64"

  config.vm.provider "virtualbox" do |vb|
    vb.memory = 1024
    vb.cpus = 1
  end

  $install_mpi = <<-SHELL
    sudo apt-get update -y
    sudo apt-get install -y build-essential openmpi-bin openmpi-common libopenmpi-dev
  SHELL

  $master_setup = <<-SHELL
    if [ ! -f /home/vagrant/.ssh/id_rsa ]; then
      ssh-keygen -t rsa -b 2048 -f /home/vagrant/.ssh/id_rsa -q -N ""
      chown vagrant:vagrant /home/vagrant/.ssh/id_rsa*
    fi
    cp /home/vagrant/.ssh/id_rsa.pub /vagrant/master_key.pub
  SHELL

  $worker_setup = <<-SHELL
    if [ -f /vagrant/master_key.pub ]; then
      mkdir -p /home/vagrant/.ssh
      cat /vagrant/master_key.pub >> /home/vagrant/.ssh/authorized_keys
      chown -R vagrant:vagrant /home/vagrant/.ssh
      chmod 600 /home/vagrant/.ssh/authorized_keys
    fi
  SHELL

  config.vm.define "master" do |master|
    master.vm.hostname = "master"
    master.vm.network "private_network", ip: "192.168.56.10"
    master.vm.provision "shell", inline: $install_mpi
    master.vm.provision "shell", inline: $master_setup, run: "always"
  end

  config.vm.define "worker1" do |worker|
    worker.vm.hostname = "worker1"
    worker.vm.network "private_network", ip: "192.168.56.11"
    worker.vm.provision "shell", inline: $install_mpi
    worker.vm.provision "shell", inline: $worker_setup, run: "always"
  end

  config.vm.define "worker2" do |worker|
    worker.vm.hostname = "worker2"
    worker.vm.network "private_network", ip: "192.168.56.12"
    worker.vm.provision "shell", inline: $install_mpi
    worker.vm.provision "shell", inline: $worker_setup, run: "always"
  end

  config.vm.define "worker3" do |worker|
    worker.vm.hostname = "worker3"
    worker.vm.network "private_network", ip: "192.168.56.13"
    worker.vm.provision "shell", inline: $install_mpi
    worker.vm.provision "shell", inline: $worker_setup, run: "always"
  end
end
