<template>
<div>
  <div class="container-fluid">
  <h1>Koken電光掲示板</h1>
  <p v-html="str" class="output-field"></p>
  <form>
  <div class="mb-3">
    <input type="text" v-model="input" class="form-control">
    </div>
    <button class="btn btn-success" @click="send">送信</button>
  </form>
  </div>
</div>
</template>

<script lang="ts">
import Vue from 'vue'
import axios from 'axios'

export default Vue.extend({
  name: 'IndexPage',
  data(){
    return{
      str: "",
      input: ""
    }
  },
  methods:{
    async send(){
      if(this.input==""){
        alert("Input something!");
      }
      const data = (await axios.post(`https://apilcd.home.k1h.dev/json/${this.input}`));
      if(data.status==200){
        console.log("OK")
        this.update();
      }else{
        console.log("Error")
      }
    },
    async update(){
      this.str="";
    const data = (await axios.get("https://apilcd.home.k1h.dev/json")).data;
    let lcdOutput:number[] = new Array();
    let i:number,j:number;
    for(j=0;j<8;j++){
      let cal = 1 << j;
      for(i=0;i<data.data.length;i++){
        if(data.data[i]&cal){
          this.str+='<span class="r-dot">●</span>';
        }else{
          this.str+='<span class="r-white">●</span>';
        }
      }
      this.str+="<br>";
    }
    }
  },
  async mounted() {
   this.update();
  }
})
</script>

<style>
body{
font-family:monospace;
}
.output-field{
  background:#201010;
  padding: 1rem;
  overflow-x: scroll;
  letter-spacing: 0.1em;
}
.r-dot{
  color: #ed5641;
  border-radius: 30px;
}
.r-white{
  color: #455151;
}

</style>