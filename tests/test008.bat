@echo First time convert YAML to JSON
marty-yaml2json test008_amazonaws-application-autoscaling-openapi.yaml      test008_amazonaws-application-autoscaling-openapi-org.json

@echo Second - convert JSON back to YAML
marty-json2yaml test008_amazonaws-application-autoscaling-openapi-org.json >test008_amazonaws-application-autoscaling-openapi-new.yaml

@echo Third - convert (YAML-JSON-YAML) to JSON
marty-yaml2json test008_amazonaws-application-autoscaling-openapi-new.yaml test008_amazonaws-application-autoscaling-openapi2-org.json

@echo Fourth - convert (YAML-JSON-YAML) to JSON using pipeline
marty-yaml2json test008_amazonaws-application-autoscaling-openapi.yaml | marty-json2yaml | marty-yaml2json >test008_amazonaws-application-autoscaling-openapi3-org.json

@call ..\run_test.bat test008 amazonaws-application-autoscaling-openapi.yaml     %* >test008_amazonaws-application-autoscaling-openapi.log.json  2>&1

