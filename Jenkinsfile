pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        sh '''
          git submodule update --init
          mkdir build 
          cd build
          cmake --build ..
        '''
      }
    }
  }
}
