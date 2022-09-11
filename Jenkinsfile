pipeline {
  agent { dockerfile true }
  stages {
    stage('Build') {
      steps {
        sh '''
          mkdir build 
          cd build
          cmake --build ..
        '''
      }
    }
  }
}
