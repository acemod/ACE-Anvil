import fs from 'node:fs'
import yaml from 'js-yaml'

export interface SiteConfig {
  name: string
  url: string
  description: string
  keywords: string
}

export interface AceConfig {
  projectUrl: string
  githubUrl: string
  discordUrl: string
  version: AceVersion
  docsVersions: DocsVersion[]
}

export interface AceVersion {
  major: number
  minor: number
  patch: number
}

export interface DocsVersion {
  name: string
  baseUrl: string
}

export const config = yaml.load(fs.readFileSync('src/config.yml', 'utf8')) as {
  site: SiteConfig
  ace: AceConfig
}
