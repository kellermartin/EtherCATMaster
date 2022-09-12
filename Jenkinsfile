pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        sh '''
        if [ -d "build" ] 
        then
          echo "Out of source build directory exists - cleaning up directory..."
          rm -d -r build
        fi
          
          git submodule update --init
          mkdir build 
          cd build
          cmake ..
          make
        '''
      }
    }
  }
}
