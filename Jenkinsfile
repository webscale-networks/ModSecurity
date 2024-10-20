/**
 * A Jenkinsfile existing in a project is seen by jenkins and used to build a
 * project. For documentation, see:
 * https://jenkins.io/doc/book/pipeline/jenkinsfile/. For variables that can
 * be used, see
 * https://jenkins.webscalenetworks.com/job/product/job/control/pipeline-syntax/globals.
 */

node {
  projectName = 'modsecurity'
  svnRepo = "file:///repo/${projectName}"
  upstreamVersion = 'v2.9.8'
  isMasterBuild = env.BRANCH_NAME == (upstreamVersion + '-webscale')

  stage('check') {
  }

  /* Checks out the main source tree */
  stage('scm') {
    /* Delete the entire directory first. */
    deleteDir()
    checkout(scm)

    /* Set a version number and a component for the debian repo.
     * The version number is determined from the most recent tag. If
     * a master build is being performed, create a tag and increment
     * the webscale version number, and use a component of "main".
     * If running a branch build, append a timestamp to the most
     * recent tag and use a component of "test".
     */
    sh('git fetch --tags')
    def last_tag = sh(
      script: 'git describe --tags --abbrev=0',
      returnStdout: true
    )
    echo('The last tag is ' + last_tag)
    def m = (last_tag =~ ~/^${upstreamVersion}(?:-webscale(\d+))?$/)
    m.find()
    revision = (m.group(1) ?: '0').toInteger()
    /* It is necessary to set the match var to null, since crossing a stage
     * boundary will attempt to serialize it and it is not serializable.
     */
    m = null
    if (isMasterBuild) {
      if (sh(script: 'git rev-list -n 1 ' + last_tag, returnStdout: true)
       != sh(script: 'git rev-list -n 1 HEAD', returnStdout: true)) {
        version = upstreamVersion + '-webscale' + (revision + 1)
        sh("git tag -f ${version}")
        sh("git push -f origin tag ${version}")
      }
      svnLoc = "${svnRepo}/${version}"
      if (sh(script: "svn info ${svnLoc} 2>/dev/null", returnStatus: true) == 0) {
        error("${version} is already published and cannot be built again")
      }
    } else {
      m = (env.BRANCH_NAME =~ ~/^${upstreamVersion}-webscale-(.*)$/)
      if (!m.find()) {
        error("Cannot decode branch name ${env.BRANCH_NAME}")
      }
      issueNumber = m.group(1)
      m = null;
      version = upstreamVersion + '-webscale' + revision +
        new Date().format('-yyyyMMddHHmmss')
      svnLoc = "${svnRepo}/${issueNumber}"
    }

    /* Set the build name and description to make it easy to identify
     * from the list of jenkins jobs.
     */
    currentBuild.displayName = version
    currentBuild.description = env.BRANCH_NAME + "<br>" + sh(
      returnStdout: true,
      script: 'git log "--pretty=format:%s (%an)" -1'
    )
  }

  /*
   * Build the thing. These commands are taken from the instructions in
   * README.md, with a modification to install locations in order to put
   * things in locations consistent with the Ubuntu packaging of
   * libpam-google-authenticator.
   */
  stage('build') {
    sh("./build.sh ${version}")
  }

  /*
   * Upload the result to subversion.
   */
  stage('publish') {
    if (!isMasterBuild) {
      sh(
        script: "svn delete ${svnLoc} -m 'Delete ${upstreamVersion}/" +
          "${issueNumber} for rebuild'",
        returnStatus: true
      )
    }
    sh("svn import --quiet --no-ignore -m 'Version ${version}' ${version} ${svnLoc}")
  }
}

