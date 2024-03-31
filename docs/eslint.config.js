import antfu from '@antfu/eslint-config'
import eslintPluginAstro from 'eslint-plugin-astro'

export default antfu(
  ...eslintPluginAstro.configs['flat/recommended'],
  {
    rules: {
      curly: ['error', 'all'],
    },
  },
)
