pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        sh '''
          rm -d -r build
          git submodule update --init
          mkdir build 
          cd build
          cmake --build ..
        '''
      }
    }
  }
}
