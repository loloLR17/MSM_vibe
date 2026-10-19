# Reprise — qualification physique limitée c36c971

Mission TR2-20261019-WINDOWS-TRUST-LIVE-RESUME ; mise à jour 2026-10-19T13:56:53.944422+00:00. Le checkpoint historique de la mission précédente est archivé sans changement ; ne pas la relancer.

- [Rapport et preuves courants](../../00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md) ; preuves locales `/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume/`.
- **Cible laissée LIVE**, BIN9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981 (c36c971), boot autonomeSTOP vérifié, finalSTOP5/17/HELD. Aucun retour M016 justifié ou effectué. STLINK003500463235510B37333439/UID004A003D/38425018/20333157.
- FRAM finale SHAe18e07adbf12176fa02f1385fa6b4713e5b35e6f101c49959229cf2f1067ec15, journal94/root107/epoch5/run13/config3gen3. Historique/queue inchangés, métadonnées exactes au modèle. Ne jamais restaurer l'ancienne FRAM2b501a ni rejouer5/12–5/17. Préserver `data\transactions.json` ; prochain couple dérivé des gardes courantes, pas d'un numéro copié ici.
- Paquet physique inchangé `C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\app\`, accepté sous SAC1 ; données `data\live.db`, HTTP5100. Service arrêté/COM libre ; lancer par [README](README.md). Acquittement c36 oscillant : token + ouverture exclusive obligatoires ; ne pas forcer. Les quatre commandes ont abouti, second run 11.85min démontrées.
- Le code8a54 corrige l'acquittement et ajoute le contrôle Windows ;239tests Service PASS. **Nouveau binaire Service.dll21a440e3…a2d17 refusé**, conservé séparément dans `TR2-LIVE-DEMO-20261019-resume`, jamais copié dans le paquet accepté. Signature durable reportée explicitement ; ne pas confondre code publié et version physiquement déployée.
- Aucun besoin de refaire les tests350+239, cross-build, campagnes physiques acquises sur code inchangé. Toute nouvelle publication Windows doit être contrôlée pour ses propres octets, aucune confiance future automatique.
- Main historique82169af,71autresworktrees et2stashes préservés. Suite : contrôle indépendant ChatGPT ; chantier séparé signature/déploiement et ergonomie éventuelle. Qualification diagnostique bornée, pas produit complet/industrie.
