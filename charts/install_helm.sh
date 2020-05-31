curl https://raw.githubusercontent.com/kubernetes/helm/master/scripts/get-helm-3 > get_helm.sh
chmod 700 get_helm.sh ; ./get_helm.sh

# --- add bitname repo
helm repo add bitnami https://charts.bitnami.com/bitnami