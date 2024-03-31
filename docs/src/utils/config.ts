import fs from 'node:fs'
import yaml from 'js-yaml'

export interface SiteConfig {
  name: string
  url: string
  description: string
  keywords: string
}

export interface AceConfig {
  githubUrl: string
  version: AceVersion
}

export interface AceVersion {
  major: number
  minor: number
  patch: number
}

export const config = yaml.load(fs.readFileSync('src/config.yml', 'utf8')) as {
  site: SiteConfig
  ace: AceConfig
}
